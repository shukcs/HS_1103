#include "RobotMgr.h"
#include <QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <QTcpSocket>
#include <qevent.h>
#include <stdio.h>
#include "FeederMgr.h"
#include "common/ModubosProtocol.h"
#include "log/DeviceLog.h"
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
    , { RobotMgr::RobotSArmed,{ Robot_StatAddr, 5} }  ///启动工程
    , { RobotMgr::ProgmaStart,{ Robot_SpeedAddr, 0} } };///工程速度

RobotMgr::RobotMgr(QObject* p) : QObject(p)
, m_modbusTcp(new ModubosProtocol(new QTcpSocket(this), Robot_ModbusAddr))
{
    connect(&FeederMgr::Instance(), &FeederMgr::actionRun, this, &RobotMgr::DoAction);
    connect(this, &RobotMgr::actionDone, &FeederMgr::Instance(), &FeederMgr::OnRobotDone);

    QSettings settings(FeederMgr::DefaultConfigFile(), QSettings::IniFormat);
    settings.beginGroup("RobotPort");
    m_ip = settings.value("ip", "192.168.1.200").toString();
    m_port = settings.value("port", 502).toInt();
    settings.endGroup();
    auto socket = (QTcpSocket*)m_modbusTcp->GetIO();
    QTimer::singleShot(50, this, [=] { RobotMgr::ConnectSocket(m_ip, m_port); });
    connect(m_modbusTcp, &ModubosProtocol::modbusRcvd, this, &RobotMgr::onRead);
    connect(socket, &QTcpSocket::disconnected, this, [=] {
        setRobotStat(PortClose);
    });
    m_idTimer = startTimer(200);
}

RobotMgr::~RobotMgr()
{
    auto socket = (QTcpSocket*)m_modbusTcp->GetIO();
    socket->blockSignals(true);
    socket->close();
}

bool RobotMgr::IsConnenct()const
{
    auto socket = (QTcpSocket*)m_modbusTcp->GetIO();
    return socket && socket->isOpen();
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
    auto socket = (QTcpSocket*)m_modbusTcp->GetIO();
    if (!socket)
        return;

    if (socket->isOpen())
    {
        if (socket->peerName() == ip && socket->peerPort() == port)
            return;
        socket->close();
    }
    socket->connectToHost(ip.isEmpty() ? m_ip : ip, port==0 ? m_port : port);
    if (!socket->waitForConnected(1000))
    {
        if (PortClose != m_comStat)
            setRobotStat(PortClose);

        return;
    }

    m_ip = socket->peerName();
    m_port = socket->peerPort();
    QSettings settings(FeederMgr::DefaultConfigFile(), QSettings::IniFormat);
    settings.beginGroup("RobotPort");
    settings.setValue("ip", m_ip);
    settings.setValue("port", (int)m_port);
    settings.endGroup();
    if (PortClose == m_comStat)
        setRobotStat(NoData);
    
    m_modbusTcp->WriteReg(Robot_StatAddr, 0);
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

QString RobotMgr::actionDescribe(RobotAction act, uint16_t a1)
{
    QString ret;
    switch (act)
    {
    case RobotMgr::OpenDoor:
        return tr("开天平门");
    case RobotMgr::CloseDoor:
        return tr("关天平门");
    case RobotMgr::MoveTube:
        return tr("夹已备料反应管%1").arg(a1);
    case RobotMgr::Tube2Stove:
        return tr("夹反应管进炉膛%1").arg(a1);
    case RobotMgr::OutStove:
        return tr("出炉膛%1").arg(a1);
    case RobotMgr::ClampStoveTube:
        return tr("夹住炉膛%1反应管").arg(a1);
    case RobotMgr::StoveTubeOut:
        return tr("夹出来炉膛%1反应管").arg(a1);
    case RobotMgr::TubeBack:
        return tr("夹反应管到回收位%1").arg(a1);
    case RobotMgr::MoveStore:
        return tr("夹料仓%1到进料器").arg(a1);
    case RobotMgr::StoreBack:
        return tr("夹进料器上料仓归位%1").arg(a1);
    case RobotMgr::BottleInBlance:
        return tr("夹料瓶%1进天平").arg(a1);
    case RobotMgr::BottleOutBlance:
        return tr("夹天平料瓶%1回支架").arg(a1);
    //case RobotMgr::Bottle2Bracket:
    //    break;
    //case RobotMgr::BottleBack:
    //    break;
    case RobotMgr::Bottle2Tube:
        return tr("料瓶倒料反应管%1").arg(a1).arg(a1);
    default:
        break;
    }
    return QString();
}

void RobotMgr::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == m_idTimer && m_modbusTcp->IsSndAll())
    {
        if (m_curAct > 0 && m_curIdx >= 0)
            ctrl();
        if (m_progmaFlag != Progma_None)
            ctrlProgma();
        else
            readStat();

        if (Communicate <= m_comStat && QDateTime::currentMSecsSinceEpoch() - m_lastTmRcv > ConnetTimeOut)
            setRobotStat(NoData);

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
        rdt = RobotSArmed; break;
    case 5:
        rdt = ProgmaStart; break;
    default:
        break;
    }
    return rdt;
}

void RobotMgr::setRobotStat(RobotStat st)
{
    if (m_comStat != st)
    {
        m_comStat = st;
        emit connectStatChanged(st);
    }
}

void RobotMgr::onRead(const uint8_t *buf, uint16_t len)
{
    m_lastTmRcv = QDateTime::currentMSecsSinceEpoch();
    if (m_comStat < Communicate)
        setRobotStat(Communicate);

    switch (buf[0])
    {
    case 3:
        if (!m_bReadRobotStat)
            prcsAction(buf, len);
        else
            prcsStat(buf, len);
        break;
    case 6:
        switch (ModubosProtocol::PichModbusU16(buf+1))
        {
        case Robot_CtrlAddr:
            m_actWrite = m_curAct;
            m_curAct = None;
            break;
        case Robot_StatAddr:
            if (Progma_Pause == m_progmaFlag)
                setRobotStat(ProgmaPause);
            else if (Progma_None==m_progmaFlag && RobotSArmed==m_comStat)
                m_comStat = ProgmaStart;
            readStat();
            break;
        case Robot_SpeedAddr:
            m_bSetSpeed = true;
            break;
        }
        break;
    }
}

void RobotMgr::readStat()
{
    if (m_comStat == PortClose)
        return;

    m_bReadRobotStat = m_comStat==ProgmaStart ? !m_bReadRobotStat : true;
    auto addr = Robot_CtrlAddr;
    if (m_bReadRobotStat)
    {
        addr = Robot_StatAddr;
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
    m_modbusTcp->ReadReg(addr);
}

void RobotMgr::initRobot()
{
    if (m_comStat == PortClose)
        return;

    auto itr = sInit.find(m_comStat);
    if (itr != sInit.end())
        m_modbusTcp->WriteReg(itr.value().addr, Robot_SpeedAddr==itr.value().addr ? m_speed : itr.value().setting);
}

void RobotMgr::prcsAction(const uint8_t *buf, uint16_t)
{
    if (ModubosProtocol::PichModbusU16(buf+2)==0 && m_actWrite>None)
    {
        auto tmp = m_actWrite;
        m_actWrite = None;
        emit actionDone(tmp);
    }
}

void RobotMgr::prcsStat(const uint8_t *buf, uint16_t)
{
    auto rdt = m_comStat;
    uint16_t st = ModubosProtocol::PichModbusU16(buf+2);
    switch (m_comStat)
    {
    case RobotMgr::PowerOff:
    case RobotMgr::PowerOn:
        if (5 == st)
        {
            rdt = PowerOn;
            if (rdt != m_comStat)
                DeviceLog::Instance() << tr("机械臂上电OK");
        }
        else if (8 == st)
        {
            rdt = RobotSArmed;
            if (rdt != m_comStat)
                DeviceLog::Instance() << tr("机械臂解锁OK");
        }
        break;
    case RobotMgr::ProgmaStart:
    case RobotMgr::ProgmaPause:
        if (0 == st)
        {
            rdt = ProgmaStart;
            if(rdt != m_comStat)
                DeviceLog::Instance() << tr("机械臂程序运行");
        }
        else if (m_progmaFlag!=Progma_None && 3==st)
        {
            rdt = Progma_Pause==m_progmaFlag ? ProgmaPause : ProgmaStart;
            m_progmaFlag = Progma_None;
            if (rdt != m_comStat)
                DeviceLog::Instance() << (ProgmaPause==rdt ? tr("机械臂程序暂停") : tr("机械臂程序继续"));
        }
        break;
    default:
        rdt = fromRead(st);
        break;
    }
    if (rdt != m_comStat)
    {
        setRobotStat(rdt);
        if (rdt != ProgmaStart)
            m_bSetSpeed = false;
        if (m_bStart && (rdt != ProgmaStart || !m_bSetSpeed))
            initRobot();
    }
}

void RobotMgr::ctrl()
{
    if (m_comStat == PortClose)
        return;

    auto tm = QDateTime::currentMSecsSinceEpoch();
    if (tm - m_lastTmCtrl >= 2000 && m_actWrite == None && m_curAct > 0 && m_curIdx >= 0)
    {
        m_modbusTcp->WriteReg(Robot_CtrlAddr, m_curAct);
        m_lastTmCtrl = tm;
        m_actWrite = None;
    }
}

void RobotMgr::ctrlProgma()
{
    if (m_progmaFlag == Progma_None)
        return;

    uint16_t st = m_progmaFlag==Progma_Pause ? 6 : 7;
    m_modbusTcp->WriteReg(Robot_StatAddr, st);
}
