#include "DevContrlMgr.h"
#include <QString>
#include <QWidget>
#include <QDebug>
#include <QTimer>
#include "portthread.h"
#include "stove/stove.h"
#include "programConfig/progItem/HeatGroupBox.h"
#include "materialFeeder/FeederMgr.h"
#include "common/ModubosProtocol.h"
#include "HsApplication.h"
#include "CtrlAction.h"

#pragma execution_character_set("utf-8")

enum {
    Pipelet_Up = 0,
    Pipelet_Down = 1,
};

bool operator==(const StepMotorStat& m1, const StepMotorStat& m2)
{
    if (m1.m_bDown != m2.m_bDown)
        return false;
    if (m1.m_bLimitH != m2.m_bLimitH)
        return false;
    if (m1.m_bLimitL != m2.m_bLimitL)
        return false;

    return m1.m_bRun == m2.m_bRun;
}

bool operator!=(const StepMotorStat& m1, const StepMotorStat& m2)
{
    return !(m1 == m2);
}

bool operator==(const ServoMotorStat& m1, const ServoMotorStat& m2)
{
    return m1.m_nPos == m2.m_nPos && m1.m_bReached == m2.m_bReached;
}
bool operator!=(const ServoMotorStat& m1, const ServoMotorStat& m2)
{
    return !(m1 == m2);
}
/****************************************************************************************
* ServoMotorStat
****************************************************************************************/
ServoMotorStat::ServoMotorStat()
{
}

ServoMotorStat::ServoMotorStat(uint8_t stat)
{
    ChangeFromStat(stat);
}

void ServoMotorStat::ChangeFromStat(uint8_t stat)
{
    m_nPos = stat & 0x7f;
    m_bReached = 0 != (stat & 0x80);
}

void ServoMotorStat::Fresh()
{
    m_nPos = 0;
    m_bReached = false;
}

int ServoMotorStat::GetPos() const
{
    return m_nPos != 0xff ? m_nPos : -1;
}

bool ServoMotorStat::IsReached() const
{
    return m_bReached;
}

ServoMotorStat& ServoMotorStat::operator=(const ServoMotorStat& m)
{
    m_nPos = m.m_nPos;
    m_bReached = m.m_bReached;
    return *this;
}

/****************************************************************************************
* portThread::StepMotorStat
****************************************************************************************/
StepMotorStat::StepMotorStat()
{
}

StepMotorStat::StepMotorStat(uint8_t stat)
{
    ChangeFromStat(stat);
}

void StepMotorStat::ChangeFromStat(uint8_t stat)
{
    m_bLimitL = (stat & 1) != 0;
    m_bLimitH = (stat & 2) != 0;
    m_bRun = (stat & 0x10) != 0;
    m_bDown = (stat & 0x20) != 0;
}

bool StepMotorStat::IsLimitL() const
{
    return m_bLimitL;
}

bool StepMotorStat::IsLimitH() const
{
    return m_bLimitH;
}

bool StepMotorStat::IsDown() const
{
    return m_bDown;
}

bool StepMotorStat::IsRun() const
{
    return m_bRun;
}

uint8_t StepMotorStat::GetType() const
{
    return m_type;
}

void StepMotorStat::SetType(uint8_t t)
{
    m_type = t;
}

uint8_t StepMotorStat::GetChannel() const
{
    return m_ch;
}

void StepMotorStat::SetChannel(uint8_t t)
{
    m_ch = t;
}

void StepMotorStat::Fresh()
{
    m_bRun = true;
}

StepMotorStat& StepMotorStat::operator=(const StepMotorStat& m)
{
    m_bLimitL = m.m_bLimitL;
    m_bLimitH = m.m_bLimitH;
    m_bRun = m.m_bRun;
    m_bDown = m.m_bDown;
    return *this;
}


/*
*   DevContrlMgr
*/
DevContrlMgr::DevContrlMgr(QObject *parent, const QString &name) : QObject(parent)
, m_timer(new QTimer(this))
{
    m_thread = new portThread;
    portName = name;
    sleepTime = 500;  // 默认0.5s采集一个点
    m_stepMotorStat[0].SetType(CtrlType::Motor_Pipelet);
    m_stepMotorStat[1].SetType(CtrlType::Motor_Pipelet);
    m_stepMotorStat[2].SetType(CtrlType::Motor_Tube);
    m_stepMotorStat[3].SetType(CtrlType::Motor_Tube);
    m_stepMotorStat[4].SetType(CtrlType::Motor_Stove);
    m_stepMotorStat[5].SetType(CtrlType::Motor_Stove);
    for (int i = 0; i < 6; ++i)
    {
        m_stepMotorStat[i].SetChannel(i % 2);
    }

    QTimer *portTimer = new QTimer(this);
    portTimer->start(1000); // 延时1s后启动相关端口
    connect(portTimer, &QTimer::timeout,this,[=](){
        com_open(true, portName);
        m_timer->start(sleepTime);
        portTimer->stop();
    });
    QTimer::singleShot(10, this, [=] {
        connect(m_thread, &portThread::port_connected, this, &DevContrlMgr::app_connected);
        connect(m_thread, &portThread::port_disconnected, this, &DevContrlMgr::app_disconnected);
        connect(m_thread, &portThread::ackRecved, this, &DevContrlMgr::onAckRecved);
        connect(m_timer, &QTimer::timeout, this, &DevContrlMgr::timer_out);
        connect(this, &DevContrlMgr::stepMotorStatChanged, hsApp->feederMgr(), &FeederMgr::OnStepMotor);
        connect(this, &DevContrlMgr::servoMotorStatChanged, hsApp->feederMgr(), &FeederMgr::OnServoMotor);
    });
}

DevContrlMgr::~DevContrlMgr()
{
    m_timer->deleteLater();
}

QByteArray DevContrlMgr::floatToBigEndian(float value) {
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);

    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << value;

    return bytes;
}

float DevContrlMgr::bigEndianToFloat(const QByteArray& bytes) {
    if (bytes.size() != 4) return 0.0f;

    QDataStream stream(bytes);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    float value;
    stream >> value;
    return value;
}

bool DevContrlMgr::com_open(bool state, const QString &name)
{
    if(state)
    {
        if(m_thread->port_setup(name))
        {
            m_thread->port_open(true);
            return true;
        }
        else {
            return false;
        }
    }
    else
    {
       m_thread->port_open(false);
       return false;
    }
}


void DevContrlMgr::app_connected()
{
     emit setConnectionState(true);
}

void DevContrlMgr::app_disconnected()
{
    emit setConnectionState(false);
    com_open(true,portName);  //  打开指定端口
}

void DevContrlMgr::strTocmd(const QString &cmd)
{
   if(cmd.isEmpty())
      return;

   QStringList strlist;
   if(cmd.contains("---"))
   {
      return ;    // 不处理功能标签
   }
   else if(cmd.contains("电磁阀") || cmd.contains("四通阀"))
   {
       strlist = cmd.split(" ");  //  以空格符分割
       uint8_t id = strlist.at(1).toInt();
       if(strlist.at(3) == "开")
       {
          swCtrl(id,true);
       }
       else if(strlist.at(3) == "关")
       {
          swCtrl(id,false);
       }
   }
   else if(cmd.contains("流量计"))
   {
        strlist = cmd.split(" ");  //  以空格符分割
        uint8_t id = strlist.at(1).toInt();
        if(cmd.contains("流量设置"))    //格式：设备 1 流量设置为 x ml/min
        {
            QString str = strlist.at(3);
            flowCtrl(id,str.toDouble());
        }
        else if(cmd.contains("短接"))    //格式：设备 1 短接 开启
        {
            QString str = strlist.at(3);
            if(str.contains("开启"))
            {
                swCtrl_flow(id,true);
            }
            else if(str.contains("关闭"))
            {
                swCtrl_flow(id,false);
            }
        }
        else if(cmd.contains("量程设置"))    //格式：设备 1 量程设置为 x ml/min
        {
            QString str = strlist.at(3);
            flowCtrl_Range(id, str.toDouble());
        }
   }
   else if(cmd.contains("背压阀"))
   {
       strlist = cmd.split(" ");  //  以空格符分割
        if(cmd.contains("状态"))    //格式：设备 1 状态设置为 全开 1
        {
            uint8_t id = strlist.at(1).toInt();
            uint8_t act = strlist.at(4).toInt();
            valve_set_manual_state(id, act);
        }
        else if(cmd.contains("模式"))    //格式：设备 1 模式 手动 1
        {
            uint8_t id = strlist.at(1).toInt();
            uint8_t act = strlist.at(4).toInt();
            valve_set_mode(id, act);
        }
        else if(cmd.contains("压力"))    //格式：设备 1 压力设置为 x MPa
        {
            uint8_t id = strlist.at(1).toInt();
            QString str = strlist.at(3);
            valve_set_pres(id, str.toFloat());
        }
   }
   else if(cmd.contains("柱塞泵"))
   {
       strlist = cmd.split(" ");  //  以空格符分割
       if(cmd.contains("流量"))    //格式：设备 1 流量设置为 x ml/min
       {
            uint8_t id = strlist.at(1).toInt();
            QString str = strlist.at(3);
            pumpSpeeed(id, str.toDouble());
       }
       else if(cmd.contains("清洗"))    //格式：设备 1 清洗 启动 1
       {
           uint8_t id = strlist.at(1).toInt();
           uint8_t act = strlist.at(4).toInt();
           pumpClean(id, act);
       }
       else if(cmd.contains("运行"))    //格式：设备 1 运行 启动 2
       {
           uint8_t id = strlist.at(1).toInt();
           uint8_t act = strlist.at(4).toInt();
           pumpClean(id, act);
       }
       else if(cmd.contains("校准系数"))    //格式：设备 1 校准系数设置为 x
       {
            strlist = cmd.split(" ");  //  以空格符分割
            uint8_t id = strlist.at(1).toInt();
            QString str = strlist.at(3);
            pumpCali(id, str.toDouble());
       }
   }
	else if(cmd.contains("液位计量程"))
	{
		strlist = cmd.split(" ");  //  以空格符分割
		QString str1 = strlist.at(1);
		QString str2 = strlist.at(3);
		liqudiCtrl_Range(str1.toInt(),str2.toDouble());
	}
	else if(cmd.contains("减压阀"))
	{
		strlist = cmd.split(" ");  //  以空格符分割
		QString str1 = strlist.at(1);
		QString str2 = strlist.at(3);
		presCtrl_Range(str1.toInt(), str2.toDouble());
	}
   else if(cmd.contains("反应炉"))
   {
       strlist = cmd.split(" ");  //  以空格符分割
       int id = strlist.at(1).toInt();

       if(cmd.contains("开始整定")) //格式：反应炉 i 开始整定 x ℃
       {
           tempAdjust(id,QString(strlist.at(3)).toInt() * 10);
       }
//       else if(cmd.contains("读取")) //格式：反应炉 i 读取
//       {
//           pidReadSave(id,0);
//       }
       else if(cmd.contains("停止整定")) //格式：反应炉 i 停止整定
       {
           tempAdjust(id,0);
       }
       else if(cmd.contains("降温")) //格式：反应炉 i 降温
       {
           tempStop(id);
       }
       else if(cmd.contains("升温至")) //格式：反应炉 i x min内从 x ℃升温至 x ℃
       {
           sloPeTemp(id,QString(strlist.at(4)).toInt(),QString(strlist.at(2)).toInt(),QString(strlist.at(6)).toInt());
       }
	}
	else if (cmd.contains("程序升温"))
	{
		programTemp(cmd);
	}
	else if (cmd.startsWith(tr("取液管 ")))
	{
		programStepMotor(cmd.split(" ", QString::SkipEmptyParts), CtrlType::Motor_Pipelet);
	}
	else if (cmd.startsWith(tr("反应管 ")))
	{
		programStepMotor(cmd.split(" ", QString::SkipEmptyParts), CtrlType::Motor_Tube);
	}
	else if (cmd.startsWith(tr("炉膛 ")))
	{
		programStepMotor(cmd.split(" ", QString::SkipEmptyParts), CtrlType::Motor_Stove);
	}
	else if (cmd.startsWith(tr("机械臂滑轨")))
	{
		programMotorRobot(cmd.split(" ", QString::SkipEmptyParts));
	}
	else if (cmd.startsWith(tr("加热套")))
	{
		programHeatMixture(cmd.split(" ", QString::SkipEmptyParts), HeatGroupBox::Dev_heat);
	}
	else if (cmd.startsWith(tr("保温箱 ")))
	{
		programHeatMixture(cmd.split(" ", QString::SkipEmptyParts), HeatGroupBox::Dev_montain);
	}
	else if (cmd.startsWith(tr("电动三通阀 ")))
	{
		programValve3Ch(cmd);
	}
	else if (cmd.contains("收集器"))
	{
		collector_conn(cmd);
	}
	else if(cmd.contains("程序温度"))
	{
			strlist = cmd.split(" ");  //  以空格符分割
			QString str1 = strlist.at(1);
			QString str2 = strlist.at(2);
			QString str3 = strlist.at(4);
			stepTemp(str1.toInt(),str2.toInt(),str3.toInt());
	}
	else if(cmd.contains("程序时间"))
	{
			strlist = cmd.split(" ");  //  以空格符分割
			QString str1 = strlist.at(1);
			QString str2 = strlist.at(2);
			QString str3 = strlist.at(4);
			stepTime(str1.toInt(),str2.toInt(),str3.toInt());
	}

	if (!m_cmdlist.isEmpty() && cmd == m_cmdlist.first())
		m_cmdlist.removeFirst();
}

void DevContrlMgr::cmdSend()
{
    if(m_cmdlist.isEmpty())  //  如果队列全部发送完了，则发送查询命令
    {
        for (int i = 0; i<5; ++i)
            board_msg_request();
    }
    else   //  队列存在未发送完成的命令
    {
        auto arr = m_cmdlist.at(0);
        m_thread->port_write((uint8_t*)arr.data(), arr.length());
    }
}

portThread *DevContrlMgr::getThread() const
{
    return m_thread;
}

const StepMotorStat* DevContrlMgr::GetStepMotorStatOf(int idx)const
{
    if (idx < 6)
        return m_stepMotorStat + idx;

    return nullptr;
}

const ServoMotorStat* DevContrlMgr::GetServoMotorStat()const
{
    return &m_servoMotorStat;
}

void DevContrlMgr::savePortName(QString name)
{
    portName = name;
}

void DevContrlMgr::timer_out()
{
    cmdSend();
}

void DevContrlMgr::setTimer(int time)
{
    sleepTime = time;
    if(sleepTime > 500)
    {
       sleepTime = 500;
    }
    if(m_timer->isActive())  // 如果已经处于计时
    {
       m_timer->start(sleepTime);
    }
}

static int16_t checkPcMsgAndLength(const uint8_t* str, uint16_t* len)
{
    uint16_t count = *len;
    *len = 0;
    for (uint16_t i = 0; i < count - 2; ++i)
    {
        if (str[i] == 0x31 && str[i + 1] < 0x31)
        {
            uint8_t posEnd = i + str[i + 1];
            if (posEnd + 2 > count)///message not complete, wait;
            {
                *len = i;
                return -1;
            }
            else
            {
                uint16_t crc = str[posEnd] << 8 | str[posEnd + 1];
                if (ModubosProtocol::ModbusCrc(str + i, posEnd-i) == crc)
                {
                    *len = posEnd + 2;
                    return i;
                }
            }
        }
        (*len) = i;
    }
    return -1;
}

void DevContrlMgr::board_msg_request()
{
    uint8_t len = 5;
    uint8_t buff[9] = { 0x31,0,1,0x80,0,0,0 };
    switch (auto t = m_countReq++ % 5)
    {
    case 0:
    case 1:
        len = 7; buff[2] = t; buff[4] = 1; break;
    case 2:
        buff[3] = 0x9a; break;
    case 3:
        buff[3] = 0x9b; break;
    case 4:
        buff[3] = 0x9c; break;
    default:
        break;
    }
    buff[1] = len;
    auto crc = ModubosProtocol::ModbusCrc(buff, len);
    buff[len] = (crc >> 8) & 0xFF;
    buff[len+1] = crc & 0xFF;
    m_thread->port_write(buff, len + 2);
}

void DevContrlMgr::swCtrl(int id, bool state)
{
    uint8_t buff[9] = {0x31,7,0x01,0x81,0x00,0x00,0x00};
    buff[4] = id;
    if(state)
    {
      buff[5] = 1;
    }
    else
    {
      buff[5] = 0;
    }
    buff[6] = 0;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::ctrlStepMotor(CtrlType::StepMotorType tp, uint8_t ch, uint8_t dir, uint16_t rpm)
{
    uint8_t cmd = 0x93;
    int idx = 0;
    switch (tp)
    {
    case CtrlType::Motor_Tube:
        idx += 2;
        cmd = 0x95;  break;
    case CtrlType::Motor_Stove:
        idx += 4;
        cmd = 0x94;  break;
    default:
        break;
    }
    rpm = (rpm & 0x3fff) | ((dir & 3)<<14);
    uint8_t buff[9] = { 0x31,7, 1, cmd, ch, rpm&0xff, rpm>>8};
    if (ch&1)
        m_stepMotorStat[idx].Fresh();
    if (ch & 2)
        m_stepMotorStat[idx+1].Fresh();

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::ctrlServoMotor(uint8_t pos)
{
    uint8_t buff[7] = { 0x31, 5, 1, 0x96, pos };
    appendToQue(buff, sizeof(buff));
    m_servoMotorStat.Fresh();
}

void DevContrlMgr::ctrValve3Ch(uint8_t ch, uint8_t dir)
{
    uint8_t buff[8] = { 0x31, 6, 1, 0x99, ch, dir };
    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::ctrlTempAndKeep(uint8_t ch, uint16_t tmp, int8_t tp)
{
    uint8_t buff[9] = { 0x31, 7, 1, tp == HeatGroupBox::Dev_heat ? 0x97 : 0x98, ch, 0, 0 };
    buff[5] = tmp & 0xff;
    buff[6] = (tmp >> 8) & 0xff;
    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::programStepMotor(const QStringList& strs, CtrlType::StepMotorType tp)
{
    uint8_t ch=0;
    uint8_t dir = 0;
    for (auto itr = strs.begin(); itr != strs.end(); )
    {
        if (*itr == tr("通道"))
        {
            while (++itr != strs.end())
            {
                bool bSuc;
                auto tmp = itr->toInt(&bSuc);
                if (!bSuc)
                    break;
                if (1 == tmp)
                    ch |= 1;
                else if (2 == tmp)
                    ch |= 2;
            }
            continue;
        }
        if (*itr == tr("刹车"))
        {
            if (++itr != strs.end())
            {
                bool bSuc;
                auto tmp = itr->toInt(&bSuc);
                if (bSuc)
                    dir |= tmp ? 2 : 0;
            }
            continue;
        }
        if (*itr == tr("方向"))
        {
            if (++itr != strs.end())
            {
                bool bSuc;
                auto tmp = itr->toInt(&bSuc);
                if (bSuc)
                    dir |= tmp ? 1 : 0;
            }
            continue;
        }
        ++itr;
    }
    ctrlStepMotor(tp, ch, dir);
}

void DevContrlMgr::programMotorRobot(const QStringList& strs)
{
    for (auto itr = strs.begin(); itr != strs.end(); )
    {
        if (*itr == tr("工作点"))
        {
            bool bSuc;
            if (++itr != strs.end())
            {
                auto tmp = itr->toInt(&bSuc);
                if (bSuc)
                    ctrlServoMotor(tmp);
                break;
            }
            continue;
        }
        ++itr;
    }
}

void DevContrlMgr::programHeatMixture(const QStringList& strs, int tp)
{
    uint16_t tmp = 0;
    uint8_t ch = 0;
    for (auto itr = strs.begin(); itr != strs.end(); )
    {
        if (*itr == tr("通道"))
        {
            bool bSuc;
            while (++itr != strs.end())
            {
                tmp = itr->toInt(&bSuc);
                if (!bSuc)
                    break;
                if (1 == tmp)
                    ch |= 1;
                else if (2 == tmp)
                    ch |= 2;
            }
            continue;
        }
        if (*itr == tr("溫度"))
        {
            bool bSuc;
            if (++itr != strs.end())
            {
                auto tmp = itr->toInt(&bSuc);
                if (!bSuc)
                    break;
            }
            continue;
        }
        tmp = 0xffff;
        ++itr;
    }
    ctrlTempAndKeep(ch, tmp, tp);
}

void DevContrlMgr::programValve3Ch(const QString& cmd)
{
    auto strs = cmd.split(" ", QString::SkipEmptyParts);
    uint8_t ch = 0;
    uint8_t dir = 0;
    for (auto itr = strs.begin(); itr != strs.end(); )
    {
        if (*itr == tr("通道"))
        {
            bool bSuc;
            while (++itr != strs.end())
            {
                auto tmp = itr->toInt(&bSuc);
                if (!bSuc)
                    break;
                if (1 == tmp)
                    ch |= 1;
                else if (2 == tmp)
                    ch |= 2;
            }
            continue;
        }
        if (*itr == tr("通向"))
        {
            bool bSuc;
            if (++itr != strs.end())
            {
                auto tmp = itr->toInt(&bSuc);
                if (!bSuc)
                    break;
                dir = tmp;
            }
            continue;
        }
        ++itr;
    }
    ctrValve3Ch(ch, dir);
}

void DevContrlMgr::swCtrl_flow(int id, bool state)
{
    uint8_t buff[9] = {0x31,7,0x01,0x90,0x00,0x00,0x00};
    buff[4] = id;
    if(state)
    {
      buff[5] = 1;
    }
    else
    {
      buff[5] = 0;
    }
    buff[6] = 0;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::flowCtrl(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x82,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::flowCtrl_Range(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x8C,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::liqudiCtrl_Range(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x8E,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::presCtrl_Range(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x8D,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::programTemp(QString cmd)
{
    QStringList strlist = cmd.split(" ");
    uint8_t buff[47] = {0x31,45,0x01,0x83,0x01,0x00,0x00};
    buff[4] = QString(strlist.at(1)).toInt();
    for (int i = 0; i < PRO_CNT; i++)
    {
        buff[5+4*i] = QString(strlist.at(2*i+2)).toInt() & 0xff;
        buff[6+4*i] = (QString(strlist.at(2*i+2)).toInt() >> 8) & 0xff;
        buff[7+4*i] = QString(strlist.at(2*i+3)).toInt() & 0xff;
        buff[8+4*i] = (QString(strlist.at(2*i+3)).toInt() >> 8) & 0xff;
    }

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::tempStop(int id)
{
    uint8_t buff[9] = {0x31,7,0x01,0x83,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = 0;
    buff[6] = 0;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::pointTemp(int id, int temp)
{
    uint8_t buff[9] = {0x31,7,0x01,0x83,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::sloPeTemp(int id, double temp1, int time, double temp2)
{
    uint16_t val = temp1 * 10;
    uint8_t buff[13] = {0x31,11,0x01,0x83,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = (val >> 8) & 0xff;

    val = time * 10;

    buff[7] = val & 0xff;
    buff[8] = (val >> 8) & 0xff;

    val = temp2 * 10;

    buff[9] = val & 0xff;
    buff[10] = (val >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::tempAdjust(int id,double temp)
{
    uint16_t val = temp * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x84,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = (val >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::stepTemp(int id, int index, int temp)
{
    uint8_t buff[10] = {0x31,8,0x01,0x87,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = index;
    buff[6] = temp & 0xff;
    buff[7] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::stepTime(int id, int index, int time)
{
    uint8_t buff[10] = {0x31,8,0x01,0x88,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = index;
    buff[6] = time & 0xff;
    buff[7] = (time >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::currentCtrl(int current)
{
    current = current * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x85,0x01,0x00,0x00};
    buff[4] = 1;
    buff[5] = current & 0xff;
    buff[6] = (current >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::SepuZhipuCtrl(int id)
{
    uint8_t buff[9] = {0x31,7,0x01,0x85,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = 1;
    buff[6] = 0;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::pumpCtrl(uint8_t id, bool state)
{
    uint8_t buff[9] = {0x31,7,0x01,0x85,0x01,0x00,0x00};
    buff[4] = id;
    if(state)
    {
      buff[5] = 1;
    }
    else
    {
      buff[5] = 0;
    }
    buff[6] = 0;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::pumpSpeeed(uint8_t id, double speed)
{
    uint16_t val = static_cast<uint16_t>(qRound(speed * 100.0));
    uint8_t buff[9] = {0x31,7,0x01,0x85,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = val >> 8;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::pumpClean(uint8_t id, uint8_t state)
{
    uint8_t buff[9] = {0x31,7,0x01,0x86,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = state;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::pumpCali(uint8_t id, double cali)
{
    uint16_t val = cali * 1000;
    uint8_t buff[9] = {0x31,7,0x01,0x8F,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = val >> 8;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::valve_set_pres(uint8_t id, float pres)
{
    uint16_t val = pres * 100;
    uint8_t buff[9] = {0x31,7,0x01,0x89,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = (val >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::valve_set_mode(uint8_t id, uint8_t mode)
{
    uint8_t buff[9] = {0x31,7,0x01,0x8A,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = mode;

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::valve_set_manual_state(uint8_t id, uint8_t mode)
{
    uint8_t buff[9] = {0x31,7,0x01,0x8B,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = mode;
    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::collector_conn(QString cmd)
{
    QStringList strlist = cmd.split(" ");
    uint16_t tmp;
    QByteArray byteArray(4, 0);
    uint8_t buff[23] = {0x31,21,0x01,0x92,0x01,0x00,0x00};
    tmp = strlist.at(3).toInt();
    buff[4] = tmp;

    tmp = strlist.at(4).toInt();
    buff[5] = (tmp >> 8) & 0xFF;;
    buff[6] = tmp & 0xFF;
    tmp = strlist.at(5).toInt();
    buff[7] = (tmp >> 8) & 0xFF;;
    buff[8] = tmp & 0xFF;
    tmp = strlist.at(6).toInt();
    buff[9] = (tmp >> 8) & 0xFF;;
    buff[10] = tmp & 0xFF;
    tmp = strlist.at(7).toInt();
    buff[11] = (tmp >> 8) & 0xFF;;
    buff[12] = tmp & 0xFF;

    byteArray = floatToBigEndian(strlist.at(8).toFloat());
    memcpy(&buff[13],byteArray.data(), 4);
    byteArray = floatToBigEndian(strlist.at(9).toFloat());
    memcpy(&buff[17],byteArray.data(), 4);

    appendToQue(buff, sizeof(buff));
}

void DevContrlMgr::appendToQue(const uint8_t* cmd, uint32_t len)
{
    QByteArray arr((const char*)cmd, len);
    auto crc = ModubosProtocol::ModbusCrc(cmd, len - 2);
    arr[len-2] = (crc >> 8) & 0xFF;
    arr[len-1] = crc & 0xFF;
    m_cmdlist << arr;
}

void DevContrlMgr::onAckRecved(const QByteArray& arr)
{
    if ((uint8_t)arr.at(2) == 0x81)
    {
        prcsMotor(arr);
        return;
    }
    if (m_cmdlist.isEmpty() || arr.size()<6)
        return;

    auto arTmp = arr.mid(4, arr.size() - 6);
    if (m_cmdlist.at(0).mid(2, arTmp.size()) == arTmp)
        m_cmdlist.takeAt(0);

    cmdSend();
    auto cmd = (uint8_t)arTmp.at(1);
    if (!m_actions.isEmpty())
    {
        auto act = m_actions.first();
        auto tp = act->getType();
        switch (tp)
        {
        case  Group_AirClear:
            {
                static QMap<uint8_t, int> sClearStep = { { 0x8D, 0}, { 0x82, 1}, { 0x8B, 2},{ 0x81, 3 } };
                auto itr = sClearStep.find(cmd);
                if (itr != sClearStep.end())
                {
                    m_stAct++;
                    doClearAir();
                }
            }
            break;
        case  Group_AirIn:
            {
                static QMap<uint8_t, int> sAirInStep = { { 0x8D, 0}, { 0x8C, 1}, { 0x8B, 2}, { 0x89, 3 }, { 0x90, 4 }, { 0x81, 5 } };
                auto itr = sAirInStep.find(cmd);
                if (itr != sAirInStep.end() && itr.value()==m_stAct)
                {
                    m_stAct++;
                    doAirIn();
                }
            }
            break;
        case  Group_LiquidIn:
            {
                static QMap<uint8_t, int> sAirInStep = { { 0x99, 1 },{ 0x85, 2 } };
                auto itr = sAirInStep.find(cmd);
                if (itr != sAirInStep.end() && itr.value() == m_stAct)
                {
                    m_stAct++;
                    doLiquidIn();
                }
            }
            break;
        case  Group_LiquidEnd:
            {
                if (0 == m_stAct && 0x85 == cmd)
                {
                    m_stAct++;
                    doLiquiEnd();
                }
            }
            break;
        case Group_AirEnd:
            {
                static QMap<uint8_t, int> sAirEnsStep = { { 0x81, 0 },{ 0x8B, 1 } };
                auto itr = sAirEnsStep.find(cmd);
                if (itr != sAirEnsStep.end() && itr.value() == m_stAct)
                {
                    m_stAct++;
                    doAirEnd();
                }
            }
            break;
        case Act_PreHeat:
        case Act_Keep:
            if (cmd == (Act_PreHeat == tp) ? 0x97 : 0x98)
            {
                m_stAct = 0;
                emit actionFinished(tp, act->GetSeq());
                m_actions.takeFirst();
                if (!m_actions.isEmpty())
                    onActionRun(m_actions.first(), true);
            }
            break;
        case Group_StoveHeat:
            if (cmd == 0x83)
            {
                m_stAct = 0;
                emit actionFinished(tp, act->GetSeq());
                m_actions.takeFirst();
                if (!m_actions.isEmpty())
                    onActionRun(m_actions.first(), true);
            }
            break;
        default:
            break;
        }
    }
}

void DevContrlMgr::prcsMotor(const QByteArray &msg)
{
    if (msg.at(3) != 11 || msg.size() != 13)
        return;

    for (int i = 0; i < 6; ++i)
    {
        StepMotorStat st(msg.at(i + 4));
        if (st != m_stepMotorStat[i])
        {
            m_stepMotorStat[i] = st;
            emit stepMotorStatChanged(m_stepMotorStat + i);
        }
        if (m_stepMotorStat[i].GetType()==CtrlType::Motor_Pipelet)
            _checkAction((m_stepMotorStat[i]));
    }
    ServoMotorStat st(msg.at(10));
    if (st != m_servoMotorStat)
    {
        m_servoMotorStat = st;
        emit servoMotorStatChanged(st.GetPos(), st.IsReached());
    }
}

void DevContrlMgr::_checkAction(const StepMotorStat &st)
{
    auto act = (st.GetType()==CtrlType::Motor_Pipelet&&m_actions.isEmpty()) ? nullptr : m_actions.first();
    if (!act) return;
    if (act->getType() == Group_LiquidIn)
    {
        auto liAct = (LiquidInAction*)act;
        if (liAct->GetChannel()==st.GetChannel() && 0==m_stAct && !st.IsDown() && st.IsLimitH())///伸出取液管
        {
            m_stAct++;
            doLiquidIn();
        }
    }
    else if (act->getType() == Group_LiquidEnd)
    {
        auto liAct = (LiquidEndAction*)act;
        if (liAct->GetChannel()==st.GetChannel() && 1==m_stAct && st.IsDown() && st.IsLimitL()) ///收回取液管
        {
            m_stAct++;
            doLiquiEnd();
        }
    }
}

void DevContrlMgr::onActionRun(const ActionAbstrctItem *act, bool bIn)
{
    if (act->GetSeq() >= 0 && !bIn)
        m_actions << act;

    switch (act->getType())
    {
    case Act_Servo:
        ctrlServoMotor(((const ServoMotorAction*)act)->GetServoPos());
        break;
    case Act_StepMotor:
        {
            auto stepAct = (const StepMotorAction*)act;
            ctrlStepMotor((CtrlType::StepMotorType)stepAct->GetMotorType(), stepAct->GetChannel() == 0 ? 1 : 2, stepAct->GetDirector() ? 1 : 0);
        }
        break;
    case Group_AirClear:
        if (m_actions.size() < 2 || m_actions.first()==act)
            doClearAir();
        break;
    case Group_AirIn:
        if (m_actions.size() < 2 || m_actions.first() == act)
            doAirIn();
        break;
    case Group_AirEnd:
        if (m_actions.size() < 2 || m_actions.first() == act)
            doAirEnd();
        break;
    case Group_LiquidIn:
        if (m_actions.size() < 2 || m_actions.first() == act)
            doLiquidIn();
        break;
    case Group_LiquidEnd:
        if (m_actions.size() < 2 || m_actions.first() == act)
            doLiquiEnd();
        break;
    case Act_PreHeat:
        {
            auto hAct = (const HeatAction*)act;
            ctrlTempAndKeep(hAct->GetTemp(), hAct->GetChannel() == 0 ? 1 : 2, HeatGroupBox::Dev_heat);
        }
        break;
    case Act_Keep:
        {
            auto hAct = (const KeepAction*)act;
            ctrlTempAndKeep(hAct->GetTemp(), hAct->GetChannel() == 0 ? 1 : 2, HeatGroupBox::Dev_montain);
        }
        break;
    case Group_StoveHeat:
        {
            auto hAct = (const StoveHeatAction*)act;
            if (hAct->UpSeconds() < 1)
                tempStop(hAct->GetChannel());
            else
                sloPeTemp(hAct->GetChannel(), hAct->GetBegTemperature(),hAct->UpMins(), hAct->GetDstTemperature());
        }
    break;
    default:
        break;
    }
}

void DevContrlMgr::doClearAir()
{
    if (auto act = (!m_actions.isEmpty() && m_actions.first()->getType()==Group_AirClear)? (const AirClrAction*)m_actions.first() : nullptr)
    {
        switch (m_stAct)
        {
        case 0:
            presCtrl_Range(act->GetChannel(), act->GetPressure()); break;//减压阀
        case 1:
            flowCtrl(act->GetChannel(), 50);///流量计
        case 2:
            valve_set_manual_state(act->GetChannel(), 1); break;//被压阀全开
        case 3:
            swCtrl(act->GetChannel(), true); break;//气路1开
        case 4:
            QTimer::singleShot(act->ClearSeconds() * 1000, this, [=] {
                swCtrl(act->GetChannel(), false);
                m_stAct = 0;
                m_actions.takeFirst();
                if (!m_actions.isEmpty())
                    onActionRun(m_actions.first(), true);
                emit actionFinished(Group_AirClear, act->GetSeq());               
            }); break;
        default:
            break;
        }
    }
}

void DevContrlMgr::doAirIn()
{
    if (auto act = (!m_actions.isEmpty() && m_actions.first()->getType() == Group_AirIn) ? (const AirInAction*)m_actions.first() : nullptr)
    {
        switch (m_stAct)
        {
        case 0:
            presCtrl_Range(act->GetChannel(), act->GetPressIn()); break;///减压 0x8D 0x8C 0x8B 0x89 0x90 0x81
        case 1:
            flowCtrl_Range(act->GetChannel(), act->GetFlow()); break;///流量计 0x8C
        case 2:
            valve_set_manual_state(act->GetChannel(), 0); break;///被压阀全开取消 0x8B
        case 3:
            valve_set_pres(act->GetChannel(), 0); break;///被压压力 0x89
        case 4:
            swCtrl_flow(act->GetChannel(), true); break;///流量计开 0x90
        case 5:
            swCtrl(act->GetChannel()+2, true); break;///气路2开 0x81
        default:
            m_stAct = 0;
            m_actions.takeFirst();
            if (!m_actions.isEmpty())
                onActionRun(m_actions.first(), true);
            emit actionFinished(Group_AirIn, act->GetSeq());
            break;
        }
    }
}

void DevContrlMgr::doLiquidIn()
{
    if (auto act = (!m_actions.isEmpty() && m_actions.first()->getType() == Group_LiquidIn) ? (const LiquidInAction*)m_actions.first() : nullptr)
    {
        switch (m_stAct)
        {
        case 0:
            ctrlStepMotor(CtrlType::Motor_Pipelet, act->GetChannel() == 0 ? 1 : 2, Pipelet_Down); break;///取液管下降 0x93
        case 1:
            ctrValve3Ch(act->GetChannel()==0 ? 1:2, 0); break;///三通阀排空 0x99 0x85 0x8B 0x89 0x90 0x81
        case 2:
            pumpSpeeed(act->GetChannel(), act->GetFlow()); break;///被压阀全开取消 0x85
        case 3:
            QTimer::singleShot(act->ClearAirSeconds() * 1000, this, [=] {
                ctrValve3Ch(act->GetChannel() == 0 ? 1 : 2, 1);
                m_stAct = 0;
                m_actions.takeFirst();
                if (!m_actions.isEmpty())
                    onActionRun(m_actions.first(), true);

                emit actionFinished(Group_LiquidIn, act->GetSeq());
            }); break;
        case 4:
            ctrlStepMotor(CtrlType::Motor_Pipelet, act->GetChannel() == 0 ? 1 : 2, 0); break;///取液管上升 0x93
        }
    }
}

void DevContrlMgr::doLiquiEnd()
{
    if (auto act = (!m_actions.isEmpty() && m_actions.first()->getType()==Group_LiquidEnd) ? (const LiquidEndAction*)m_actions.first() : nullptr)
    {
        switch (m_stAct)
        {
        case 0:
            pumpSpeeed(act->GetChannel(), 0); break;///被压阀全开取消 0x85
        case 1:
            ctrlStepMotor(CtrlType::Motor_Pipelet, act->GetChannel() == 0 ? 1 : 2, Pipelet_Up); break;///取液管上升 0x93
        default:
            m_stAct = 0;
            m_actions.takeFirst();
            if (!m_actions.isEmpty())
                onActionRun(m_actions.first(), true);
            emit actionFinished(Group_LiquidEnd, act->GetSeq());
            break;
        }
    }
}

void DevContrlMgr::doAirEnd()
{
    if (auto act = (!m_actions.isEmpty() && m_actions.first()->getType() == Group_AirEnd) ? (const AirEndAction*)m_actions.first() : nullptr)
    {
        switch (m_stAct)
        {
        case 0:
            swCtrl(act->GetChannel() + 2, false); break;///气路2开 0x81 0x8B
        case 1:
            valve_set_manual_state(act->GetChannel(), 1); break;///被压阀全开 0x8B
        default:
            m_stAct = 0;
            m_actions.takeFirst();
            if (!m_actions.isEmpty())
                onActionRun(m_actions.first(), true);
            emit actionFinished(Group_AirEnd, act->GetSeq());
            break;
        }
    }
}
