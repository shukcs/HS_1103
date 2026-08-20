#include "RobotMgr.h"
#include <QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <QTcpSocket>
#include <qevent.h>
#include "FeederDecoder.h"
#pragma execution_character_set("utf-8")

enum
{
    Robot_ModbusAddr = 1,
    Robot_CtrlAddr = 300,
    Robot_StatAddr = 1400,
    Robot_SpeedAddr = 1401,
    Robot_RobotStatAddr = 1303,
    Robot_ProgmaStatAddr = 1202,
    ConnetTimeOut = 30000,
};

struct InitAct{
    uint16_t addr;
    uint8_t  setting;
};

static QMap<RobotMgr::RobotStat, InitAct> sInit = { { RobotMgr::PowerOff,{ Robot_StatAddr, 2} }
    , {RobotMgr::PowerOn, { Robot_StatAddr, 3}} ///机器臂启动
    , { RobotMgr::RobotStart,{ Robot_StatAddr, 5} }  ///启动工程
    , { RobotMgr::ProgmaStart,{ Robot_SpeedAddr, 0} } };///工程速度

RobotMgr::RobotMgr(QObject* p) : QObject(p), m_socket(new QTcpSocket(this))
{
    connect(&FeederMgr::Instance(), &FeederMgr::actionRun, this, &RobotMgr::DoAction);
    connect(this, &RobotMgr::actionDone, &FeederMgr::Instance(), &FeederMgr::OnRobotDone);
    QSettings settings(FeederMgr::DefaultConfigFile(), QSettings::IniFormat);
    settings.beginGroup("RobotPort");
    m_ip = settings.value("ip", "192.168.1.200").toString();
    m_port = settings.value("port", 502).toInt();
    settings.endGroup();
    ConnectSocket();
    connect(m_socket, &QTcpSocket::readyRead, this, &RobotMgr::onRead);
    connect(m_socket, &QTcpSocket::disconnected, this, [=] {
        m_comStat = PortClose;
        emit connectStatChanged(m_comStat);
    });
    m_idTimer = startTimer(200);
}

RobotMgr::~RobotMgr()
{
    m_socket->blockSignals(true);
    m_socket->close();
}

bool RobotMgr::IsConnenct()const
{
    return m_socket->isOpen();
}

void RobotMgr::DoAction(const DeviceAct *act)
{
    if (Dev_Robot == act->type)
    {
        m_curAct = (RobotAction)act->robotStep;
        m_curIdx = act->robotIndex;
        QTimer::singleShot(200, this, &RobotMgr::ctrl);
    }
}

void RobotMgr::ConnectSocket(const QString &ip, uint16_t port)
{
    if (m_socket->isOpen())
    {
        if (m_socket->peerName() == ip && m_socket->peerPort() == port)
            return;
        m_socket->close();
    }
    m_socket->connectToHost(ip.isEmpty() ? m_ip : ip, port==0 ? m_port : port);
    if (!m_socket->waitForConnected(1000))
    {
        if (PortClose != m_comStat)
        {
            m_comStat = PortClose;
            emit connectStatChanged(PortClose);
        }
        return;
    }

    m_ip = m_socket->peerName();
    m_port = m_socket->peerPort();
    QSettings settings(FeederMgr::DefaultConfigFile(), QSettings::IniFormat);
    settings.beginGroup("RobotPort");
    settings.setValue("ip", m_ip);
    settings.setValue("port", (int)m_port);
    settings.endGroup();
    if (PortClose == m_comStat)
    {
        m_comStat = NoData;
        emit connectStatChanged(m_comStat);
    }
    writeInit();
}

RobotMgr::RobotStat RobotMgr::tcpSocketStat() const
{
    return m_comStat;
}

const QString & RobotMgr::GetHost() const
{
    return m_ip;
}

uint16_t RobotMgr::GetPort() const
{
    return m_port;
}

bool RobotMgr::IsProgmaRun() const
{
    return ProgmaStart == m_comStat;
}

void RobotMgr::SetPause()
{
    if (ProgmaStart == m_comStat)
        m_progmaFlag = Progma_Pause;
    else if (ProgmaPause)
        m_progmaFlag = Progma_Contniue;
    ctrlProgma();
}

void RobotMgr::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == m_idTimer)
    {
        if (m_curAct > 0 && m_curIdx >= 0)
            ctrl();
        if (m_progmaFlag != Progma_None)
            ctrlProgma();
        else
            readStat();

        if (Communicate<=m_comStat && QDateTime::currentMSecsSinceEpoch()-m_lastTmRcv>ConnetTimeOut)
        {
            m_comStat = NoData;
            emit connectStatChanged(NoData);
        }
        return;
    }
    QObject::timerEvent(e);
}

RobotMgr::RobotStat RobotMgr::fromRead(uint16_t st)
{
    RobotStat rdt = PowerOff;
    switch (st)
    {
    case 2:
        rdt = PowerOn; break;
    case 3:
        rdt = RobotStart; break;
    case 5:
        rdt = ProgmaStart; break;
    default:
        break;
    }
    return rdt;
}

QByteArray RobotMgr::pickTcpModbus()
{
    int idx = 6;
    while (1)
    {
        idx = m_rcvs.indexOf(char(Robot_ModbusAddr), idx);
        if (idx < 0)
        {
            if (m_rcvs.size() > 6)
                m_rcvs = m_rcvs.right(6);
            return QByteArray();
        }
        auto remian = m_rcvs.size() - idx;
        if (remian < 4)
            break;
        auto data = (uint8_t*)m_rcvs.data() + idx;
        auto len = getAckLen(data, m_rcvs.size() - idx);
        if (remian < len)
            break;

        if (len == FeederMgr::PichModbusU32(data-4))
        {
            auto arr = m_rcvs.mid(idx, len);
            m_rcvs.remove(0, len + idx);
            return arr;
        }
        ++idx;
    }
    if (idx > 0)
        m_rcvs.remove(0, idx);

    return QByteArray();
}

int32_t RobotMgr::getAckLen(const uint8_t* buf, uint32_t)
{
    switch (buf[1])
    {
    case 6:
        return 6;
    case 1:
    case 3:
        return buf[2] + 3;
    }

    return -1;
}

void RobotMgr::onRead()
{
    if (m_socket->bytesAvailable() < 1)
        return;

    m_rcvs += m_socket->readAll();
    auto mdbs = pickTcpModbus();
    if (!mdbs.isEmpty())
    {
        m_bWait = false;
        switch (mdbs[1])
        {
        case 3:
            if (!m_bReadRobotStat)
                prcsAction(mdbs);
            else
                prcsStat(mdbs);
            break;
        case 6:
            switch (FeederMgr::PichModbusU16(mdbs.data() + 2))
            {
            case Robot_CtrlAddr:
                m_actWrite = m_curAct;
                m_curAct = None;
                break;
            case Robot_StatAddr:
                if (Progma_None != m_progmaFlag)
                    m_progmaFlag = Progma_None;
                else if (RobotStart == m_comStat)
                    m_comStat = ProgmaStart;
                readStat();
                break;
            case Robot_SpeedAddr:
                m_bSetSpeed = true;
                break;
            }
            break;
        }
        m_lastTmRcv = QDateTime::currentMSecsSinceEpoch();
        if (m_comStat < Communicate)
        {
            m_comStat = Communicate;
            emit connectStatChanged(Communicate);
        }
    }
}

void RobotMgr::readStat()
{
    if (m_comStat == PortClose || m_bWait)
        return;

    m_bReadRobotStat = m_comStat == ProgmaStart ? !m_bReadRobotStat : true;
    uint8_t buff[12] = {0,0,0,0,0,0, Robot_ModbusAddr, 3,0,0,0,1 };
    FeederMgr::AddModbusData(buff, m_seq++);
    FeederMgr::AddModbusU32(buff + 2, 6);
    auto addr = Robot_CtrlAddr;
    if (m_bReadRobotStat)
    {
        switch (m_comStat)
        {
        case RobotMgr::PowerOff:
        case RobotMgr::PowerOn:
            addr = Robot_RobotStatAddr;
            break;
        case ProgmaStart:
        case ProgmaPause:
            addr = Robot_ProgmaStatAddr;
            break;
        }
    }
    FeederMgr::AddModbusData(buff+ 8, addr);
    m_socket->write((char*)buff, sizeof(buff));
    m_bWait = true;
}

void RobotMgr::initRobot()
{
    if (m_comStat == PortClose || m_bWait)
        return;

    auto itr = sInit.find(m_comStat);
    if (itr != sInit.end())
    {
        uint8_t buff[12] = { 0,0,0,0,0,0, Robot_ModbusAddr, 6,0,0,0,0 };
        FeederMgr::AddModbusData(buff, m_seq++);
        FeederMgr::AddModbusU32(buff + 2, 6);
        FeederMgr::AddModbusData(buff + 8, itr.value().addr);
        if (Robot_SpeedAddr == itr.value().addr)
            FeederMgr::AddModbusData(buff + 10, m_speed);
        else
            FeederMgr::AddModbusData(buff + 10, itr.value().setting);

        m_socket->write((char*)buff, sizeof(buff));
        m_bWait = true;
    }
}

void RobotMgr::prcsAction(const QByteArray &msg)
{
    if (FeederMgr::PichModbusU16(msg.data() + 3) == 0 && m_actWrite > None)
    {
        auto tmp = m_actWrite;
        m_actWrite = None;
        emit actionDone(tmp);
    }
}

void RobotMgr::prcsStat(const QByteArray &msg)
{
    auto rdt = m_comStat;
    uint16_t st = FeederMgr::PichModbusU16(msg.data() + 3);
    switch (m_comStat)
    {
    case RobotMgr::PowerOff:
    case RobotMgr::PowerOn:
        if (5 == st)
            rdt = PowerOn;
        else if (8 == st)
            rdt = RobotStart;
        break;
    case RobotMgr::ProgmaStart:
    case RobotMgr::ProgmaPause:
        if (0 == st)
            rdt = ProgmaStart;
        else if (2 == st)
            rdt = ProgmaPause;
        break;
    default:
        rdt = fromRead(st);
        break;
    }
    if (rdt != m_comStat)
    {
        m_comStat = rdt;
        emit connectStatChanged(rdt);
        if (rdt != ProgmaStart)
            m_bSetSpeed = false;
        if (m_bStart && (rdt != ProgmaStart || !m_bSetSpeed))
            initRobot();
    }
}

void RobotMgr::writeInit()
{
    uint8_t buff[12] = { 0,0,0,0,0,0, Robot_ModbusAddr, 6,0,0,0,0 };
    FeederMgr::AddModbusData(buff, m_seq++);
    FeederMgr::AddModbusU32(buff + 2, 6);
    FeederMgr::AddModbusData(buff + 8, Robot_StatAddr);

    m_socket->write((char*)buff, sizeof(buff));
}

void RobotMgr::ctrl()
{
    if (m_comStat == PortClose || m_bWait)
        return;

    auto tm = QDateTime::currentMSecsSinceEpoch();
    if (tm - m_lastTmCtrl >= 2000 && m_actWrite == None && m_curAct > 0 && m_curIdx >= 0)
    {
        int run_num = m_curAct/* + m_curIdx*/;
        uint8_t buff[12] = { 0,0,0,0,0,0, Robot_ModbusAddr, 6,0,0,0,0 };
        FeederMgr::AddModbusData(buff, m_seq++);
        FeederMgr::AddModbusU32(buff + 2, 6);
        FeederMgr::AddModbusData(buff + 8, Robot_CtrlAddr);
        FeederMgr::AddModbusData(buff + 10, run_num);
        m_socket->write((char*)buff, sizeof(buff));
        m_lastTmCtrl = tm;
        m_bWait = true;
        m_actWrite = None;
    }
}

void RobotMgr::ctrlProgma()
{
    if (m_progmaFlag == Progma_None)
        return;

    uint16_t st = m_progmaFlag == Progma_Pause ? 6 : 7;
    uint8_t buff[12] = { 0,0,0,0,0,0, Robot_ModbusAddr, 6,0,0,0,0 };
    FeederMgr::AddModbusData(buff, m_seq++);
    FeederMgr::AddModbusU32(buff + 2, 6);
    FeederMgr::AddModbusData(buff + 8, Robot_StatAddr);
    FeederMgr::AddModbusData(buff + 10, st);
    m_socket->write((char*)buff, sizeof(buff));
}
