#include "strdecoder.h"
#include <QString>
#include <QWidget>
#include <QDebug>
#include <QTimer>
#include "portthread.h"
#include "stove/stove.h"
#include "programConfig/HeatGroupBox.h"
#include "materialFeeder/FeederDecoder.h"
#pragma execution_character_set("utf-8")

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
*   strDecoder
*/
static strDecoder *sStrDecoder = nullptr;
strDecoder::strDecoder(QObject *parent, const QString &name) : QObject(parent)
, m_timer(new QTimer(this))
{
    thread = new portThread;
    portName = name;
    sleepTime = 500;  // 默认0.5s采集一个点
    connect(thread, &portThread::port_connected, this, &strDecoder::app_connected);
    connect(thread, &portThread::port_disconnected, this, &strDecoder::app_disconnected);
    connect(thread, &portThread::ackRecved, this, &strDecoder::onAckRecved);
    connect(m_timer, &QTimer::timeout, this, &strDecoder::timer_out);
	connect(&FeederDecoder::Instance(), &FeederDecoder::actionRun, this, &strDecoder::onActionRun);
	connect(&FeederDecoder::Instance(), &FeederDecoder::feedTubeChanged, this, &strDecoder::onFeedTubeChanged);
    connect(this, &strDecoder::stepMotorStatChanged, &FeederDecoder::Instance(), &FeederDecoder::OnStepMotor);
    connect(this, &strDecoder::servoMotorStatChanged, &FeederDecoder::Instance(), &FeederDecoder::OnServoMotor);
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
    sStrDecoder = this;
}

strDecoder::~strDecoder()
{
    m_timer->deleteLater();
}

QByteArray strDecoder::floatToBigEndian(float value) {
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);

    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << value;

    return bytes;
}

float strDecoder::bigEndianToFloat(const QByteArray& bytes) {
    if (bytes.size() != 4) return 0.0f;

    QDataStream stream(bytes);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    float value;
    stream >> value;
    return value;
}

strDecoder* strDecoder::Instance()
{
    return sStrDecoder;
}

bool strDecoder::com_open(bool state, const QString &name)
{
    if(state)
    {
        if(thread->port_setup(name))
        {
            thread->port_open(true);
            return true;
        }
        else {
            return false;
        }
    }
    else
    {
       thread->port_open(false);
       return false;
    }
}


void strDecoder::app_connected()
{
     emit setConnectionState(true);
}

void strDecoder::app_disconnected()
{
    emit setConnectionState(false);
    com_open(true,portName);  //  打开指定端口
}

void strDecoder::onProgramRun(const QString &cmd)
{
	bool bSnd = m_programList.isEmpty();
    m_programList.append(cmd);
	if (bSnd)
		strTocmd(m_programList.first());
}

void strDecoder::strTocmd(const QString &cmd)
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
	else if(cmd.contains("压力量程"))
	{
		strlist = cmd.split(" ");  //  以空格符分割
		QString str1 = strlist.at(1);
		QString str2 = strlist.at(3);
		presCtrl_Range(str1.toInt(),str2.toDouble());
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
	else if(cmd.contains("曲线记录"))
	{
		strlist = cmd.split(" ");  //  以空格符分割
		if(strlist.at(1) == "开始")
		{
			emit startRecord(true);
		}
		else if(strlist.at(1) == "停止")
		{
			emit startRecord(false);
		}
	}
	else if(cmd.contains("曲线数据保存"))
	{
		emit autoSavedata();
	}
	else if (cmd.startsWith(tr("固体投料")))
	{
		strlist = cmd.split(" ", QString::SkipEmptyParts);  //  以空格符分割
		if (strlist.size() > 2)
		{
			if (strlist.at(1) == tr("装载炉膛"))
			{
				auto ch = strlist.at(2).toInt();
				QMap<QString, float> feeds;
				for (int i = 3; i+1 < strlist.size(); i+=2)
				{
					feeds[strlist.at(i)] = strlist.at(i+1).toFloat();
				}
				if (!FeederDecoder::Instance().FeedSolidMaterial(feeds, -1, -1, ch) && !m_cmdlist.isEmpty())
					m_cmdlist.removeFirst();
			}
			else if (strlist.at(1) == tr("收回反应管"))
			{
				if (auto tb = FeederDecoder::Instance().GetInSotveTube(strlist.at(2).toInt()))
				{
					if (!FeederDecoder::Instance().StoveTubeBack(tb->numb) && !m_cmdlist.isEmpty())
						m_cmdlist.removeFirst();
				}
			}
			else if (!m_cmdlist.isEmpty())
			{
				m_cmdlist.removeFirst();
			}
		}
		return;
	}
	if (!m_cmdlist.isEmpty() && cmd == m_cmdlist.first())
		m_cmdlist.removeFirst();
}

void strDecoder::cmdSend()
{
    if(m_cmdlist.isEmpty())  //  如果队列全部发送完了，则发送查询命令
    {
        for (int i = 0; i<5; ++i)
            board_msg_request();
    }
    else   //  队列存在未发送完成的命令
    {
        auto arr = m_cmdlist.at(0);
        thread->port_write((uint8_t*)arr.data(), arr.length());
    }
}

portThread *strDecoder::getThread() const
{
    return thread;
}

const StepMotorStat* strDecoder::GetStepMotorStatOf(int idx)const
{
    if (idx < 6)
        return m_stepMotorStat + idx;

    return nullptr;
}

const ServoMotorStat* strDecoder::GetServoMotorStat()const
{
    return &m_servoMotorStat;
}

void strDecoder::savePortName(QString name)
{
    portName = name;
}

void strDecoder::timer_out()
{
    cmdSend();
}

void strDecoder::setTimer(int time)
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
                if (FeederDecoder::Modbus_crc16(str + i, posEnd-i) == crc)
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

void strDecoder::board_msg_request()
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
    auto crc = FeederDecoder::Modbus_crc16(buff, len);
    buff[len] = (crc >> 8) & 0xFF;
    buff[len+1] = crc & 0xFF;
    thread->port_write(buff, len + 2);
}

void strDecoder::swCtrl(int id, bool state)
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

void strDecoder::ctrlStepMotor(CtrlType::StepMotorType tp, uint8_t ch, uint8_t dir, uint16_t rpm)
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

void strDecoder::ctrlServoMotor(uint8_t pos)
{
    uint8_t buff[7] = { 0x31, 5, 1, 0x96, pos };
    appendToQue(buff, sizeof(buff));
    m_servoMotorStat.Fresh();
}

void strDecoder::programStepMotor(const QStringList& strs, CtrlType::StepMotorType tp)
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

void strDecoder::programMotorRobot(const QStringList& strs)
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

void strDecoder::programHeatMixture(const QStringList& strs, int tp)
{
    uint8_t buff[9] = { 0x31, 7, 1, tp==HeatGroupBox::Dev_heat?0x97:0x98, 0, 0, 0};
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
                    buff[4] |= 1;
                else if (2 == tmp)
                    buff[4] |= 2;
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
                buff[5] = tmp & 0xff;
                buff[6] = (tmp >> 8) & 0xff;
            }
            continue;
        }
        if (*itr == tr("停止加热"))
        {
            buff[5] = 0xff;
            buff[6] = 0xff;
        }
        ++itr;
    }
    appendToQue(buff, sizeof(buff));
}

void strDecoder::programValve3Ch(const QString& cmd)
{
    uint8_t buff[8] = { 0x31, 7, 1, 0x99, 0, 0};
    auto strs = cmd.split(" ", QString::SkipEmptyParts);
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
                    buff[4] |= 1;
                else if (2 == tmp)
                    buff[4] |= 2;
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
                buff[5] = tmp;
            }
            continue;
        }
        ++itr;
    }
    appendToQue(buff, sizeof(buff));
}

void strDecoder::swCtrl_flow(int id, bool state)
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

void strDecoder::flowCtrl(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x82,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::flowCtrl_Range(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x8C,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::liqudiCtrl_Range(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x8E,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::presCtrl_Range(int id, double flow)
{
    uint16_t temp = flow * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x8D,0x01,0x00,0x00};

    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::programTemp(QString cmd)
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

void strDecoder::tempStop(int id)
{
    uint8_t buff[9] = {0x31,7,0x01,0x83,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = 0;
    buff[6] = 0;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::pointTemp(int id, int temp)
{
    uint8_t buff[9] = {0x31,7,0x01,0x83,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = temp & 0xff;
    buff[6] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::sloPeTemp(int id, double temp1, int time, double temp2)
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

void strDecoder::tempAdjust(int id,double temp)
{
    uint16_t val = temp * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x84,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = (val >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::stepTemp(int id, int index, int temp)
{
    uint8_t buff[10] = {0x31,8,0x01,0x87,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = index;
    buff[6] = temp & 0xff;
    buff[7] = (temp >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::stepTime(int id, int index, int time)
{
    uint8_t buff[10] = {0x31,8,0x01,0x88,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = index;
    buff[6] = time & 0xff;
    buff[7] = (time >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::currentCtrl(int current)
{
    current = current * 10;
    uint8_t buff[9] = {0x31,7,0x01,0x85,0x01,0x00,0x00};
    buff[4] = 1;
    buff[5] = current & 0xff;
    buff[6] = (current >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::SepuZhipuCtrl(int id)
{
    uint8_t buff[9] = {0x31,7,0x01,0x85,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = 1;
    buff[6] = 0;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::pumpCtrl(uint8_t id, bool state)
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

void strDecoder::pumpSpeeed(uint8_t id, double speed)
{
    uint16_t val = static_cast<uint16_t>(qRound(speed * 100.0));
    uint8_t buff[9] = {0x31,7,0x01,0x85,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = val >> 8;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::pumpClean(uint8_t id, uint8_t state)
{
    uint8_t buff[9] = {0x31,7,0x01,0x86,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = state;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::pumpCali(uint8_t id, double cali)
{
    uint16_t val = cali * 1000;
    uint8_t buff[9] = {0x31,7,0x01,0x8F,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = val >> 8;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::valve_set_pres(uint8_t id, float pres)
{
    uint16_t val = pres * 100;
    uint8_t buff[9] = {0x31,7,0x01,0x89,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = val & 0xff;
    buff[6] = (val >> 8) & 0xff;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::valve_set_mode(uint8_t id, uint8_t mode)
{
    uint8_t buff[9] = {0x31,7,0x01,0x8A,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = mode;

    appendToQue(buff, sizeof(buff));
}

void strDecoder::valve_set_manual_state(uint8_t id, uint8_t mode)
{
    uint8_t buff[9] = {0x31,7,0x01,0x8B,0x01,0x00,0x00};
    buff[4] = id;
    buff[5] = mode;
    appendToQue(buff, sizeof(buff));
}

void strDecoder::collector_conn(QString cmd)
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

void strDecoder::appendToQue(const uint8_t* cmd, uint32_t len)
{
    QByteArray arr((const char*)cmd, len);
    auto crc = FeederDecoder::Modbus_crc16(cmd, len - 2);
    arr[len-2] = (crc >> 8) & 0xFF;
    arr[len-1] = crc & 0xFF;
    m_cmdlist << arr;
}

void strDecoder::onAckRecved(const QByteArray& arr)
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
}

void strDecoder::prcsMotor(const QByteArray &msg)
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
    }
    ServoMotorStat st(msg.at(10));
    if (st != m_servoMotorStat)
    {
        m_servoMotorStat = st;
        emit servoMotorStatChanged(st.GetPos(), st.IsReached());
    }
}

void strDecoder::onActionRun(const DeviceAct *act)
{
    if (Dev_Servo == act->type)
        ctrlServoMotor(act->servoPos);
    else if (Dev_StepMotor == act->type)
        ctrlStepMotor((CtrlType::StepMotorType)act->stepType, act->stepCh==0?1:2, act->stepDirCont ? 1 : 0);
}

void strDecoder::onFeedTubeChanged(uint8_t ch, bool bFixed)
{
	auto cmd = m_cmdlist.isEmpty() ? QString() : m_cmdlist.first();
	if (cmd.startsWith(tr("固体投料")))
	{
		auto strlist = cmd.split(" ", QString::SkipEmptyParts);  //  以空格符分割
		if (strlist.size() > 2)
		{
			if (strlist.at(1) == tr("装载炉膛") && strlist.at(2).toInt()==ch)
			{
				m_cmdlist.removeFirst();
			}
			else if (strlist.at(1) == tr("收回反应管") && strlist.at(2).toInt()==ch)
			{
				m_cmdlist.removeFirst();
			}
		}
	}
}

