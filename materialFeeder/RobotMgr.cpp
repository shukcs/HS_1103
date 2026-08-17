#include "RobotMgr.h"
#include <QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <qevent.h>
#include "FeederDecoder.h"
#pragma execution_character_set("utf-8")

enum
{
    Robot_ModbusAddr = 1,
    Robot_CtrlAddr = 300,
    ConnetTimeOut = 3000,
};

RobotMgr::RobotMgr(QObject* p) : QObject(p), m_port(new QSerialPort(this))
{
    connect(m_port, &QSerialPort::readyRead, this, &RobotMgr::onRead);
    connect(&FeederDecoder::Instance(), &FeederDecoder::actionRun, this, &RobotMgr::DoAction);
    connect(this, &RobotMgr::actionDone, &FeederDecoder::Instance(), &FeederDecoder::OnRobotDone);
    QSettings settings(FeederDecoder::DefaultConfigFile(), QSettings::IniFormat);
    settings.beginGroup("RobotPort");
    m_portName = settings.value("port", "COM1").toString();
    auto baut = settings.value("baut", 115200).toInt();
    settings.endGroup();
    m_port->setPortName(m_portName);
    m_port->setBaudRate(baut);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    connect(m_port, &QSerialPort::baudRateChanged, this, [=] {m_bPortChaned = true; });
    m_port->open(QSerialPort::ReadWrite);
    QTimer::singleShot(50, this, &RobotMgr::ConnectPort);
    m_idTimer = startTimer(500);
}

RobotMgr::~RobotMgr()
{
    m_port->close();
}

bool RobotMgr::IsConnenct()const
{
    return m_port->isOpen();
}

void RobotMgr::DoAction(const DeviceAct *act)
{
    if (Dev_Robot == act->type)
    {
        m_curAct = (RobotAction)act->robotStep;
        m_curIdx = act->robotIndex;
        QTimer::singleShot(50, this, &RobotMgr::ctrl);
    }
}

void RobotMgr::ConnectPort()
{
    if (!m_port->isOpen())
    {
        if (PortClose != m_comStat)
        {
            m_comStat = PortClose;
            emit connectStatChanged(PortClose);
        }
        m_portName = m_port->portName();
        return;
    }

    if (m_portName != m_port->portName() || m_bPortChaned)
    {
        m_portName = m_port->portName();
        QSettings settings(FeederDecoder::DefaultConfigFile(), QSettings::IniFormat);
        settings.beginGroup("RobotPort");
        settings.setValue("port", m_portName);
        settings.setValue("baut", (int)m_port->baudRate());
        settings.endGroup();
        m_bPortChaned = false;
    }
    if (PortClose == m_comStat)
    {
        m_comStat = NoData;
        emit connectStatChanged(m_comStat);
    }
}

QSerialPort * RobotMgr::serialPort() const
{
    return m_port;
}

RobotMgr::PortStat RobotMgr::serialPortStat() const
{
    return m_comStat;
}

void RobotMgr::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == m_idTimer)
    {
        if (m_stat==1 && m_curAct > 0 && m_curIdx >= 0)
            ctrl();
        else
            readStat();

        if (Communicate == m_comStat && QDateTime::currentMSecsSinceEpoch() - m_lastTmRcv > ConnetTimeOut)
        {
            m_comStat = NoData;
            emit connectStatChanged(NoData);
        }
        return;
    }
    QObject::timerEvent(e);
}

void RobotMgr::ctrl()
{
    auto tm = QDateTime::currentMSecsSinceEpoch();
    if (tm-m_lastTmCtrl>=2000 && m_actWrite==None && m_curAct > 0 && m_curIdx >= 0)
    {
        int run_num = m_curAct + m_curIdx;
        m_stat = 1;
        uint8_t buff[8] = { Robot_ModbusAddr, 6,0,0,0,0 };
        FeederDecoder::AddModbusData(buff + 2, Robot_CtrlAddr);
        FeederDecoder::AddModbusData(buff + 4, run_num);
        FeederDecoder::AddModbusData(buff + 6, FeederDecoder::Modbus_crc16(buff, 6));
        m_port->write((char*)buff, 8);
        m_lastTmCtrl = tm;
    }
}

QByteArray RobotMgr::pickModbus()
{
    int idx = 0;
    while (1)
    {
        idx = m_rcvs.indexOf(char(Robot_ModbusAddr), idx);
        if (idx < 0)
        {
            m_rcvs.clear();
            return QByteArray();
        }
        auto remian = m_rcvs.size() - idx;
        if (remian < 4)
            break;
        auto data = (uint8_t*)m_rcvs.data() + idx;
        auto len = getAckLen(data, m_rcvs.size() - idx);
        if (remian < len)
            break;

        uint16_t crc = FeederDecoder::PichModbusU16(data + len - 2);
        if (crc == FeederDecoder::Modbus_crc16(data, len - 2))
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

uint32_t RobotMgr::getAckLen(const uint8_t* buf, uint32_t)
{
    switch (buf[1])
    {
    case 6:
        return 8;
    case 1:
    case 3:
        return buf[2] + 5;
    }

    return 5;
}

void RobotMgr::onRead()
{
    m_rcvs += m_port->readAll();
    auto mdbs = pickModbus();
    if (!mdbs.isEmpty())
    {
        if (3 == mdbs[1])
        { 
            auto st = FeederDecoder::PichModbusU16(mdbs.data() + 3);
            if (st==0 && m_stat==2 && m_actWrite>None)
            {
                auto tmp = m_actWrite;
                m_actWrite = None;
                m_stat = 0;
                emit actionDone(tmp);
            }
        }
        else if (6 == mdbs[1])
        {
            m_actWrite = m_curAct;
            m_stat = 2;
        }
        m_lastTmRcv = QDateTime::currentMSecsSinceEpoch();
        if (m_comStat != Communicate)
        {
            m_comStat = Communicate;
            emit connectStatChanged(Communicate);
        }
    }

}
bool RobotMgr::parse()
{
    return false;
}

void RobotMgr::readStat()
{
    if (!m_port->isOpen())
        return;

    uint8_t buff[8] = { Robot_ModbusAddr, 3,0,0,0,1 };
    FeederDecoder::AddModbusData(buff + 2, Robot_CtrlAddr);
    FeederDecoder::AddModbusData(buff + 6, FeederDecoder::Modbus_crc16(buff, 6));
    m_port->write((char*)buff, 8);
}