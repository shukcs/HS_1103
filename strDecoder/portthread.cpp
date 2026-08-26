#include "portthread.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QWidget>
#include <QDebug>
#include <QApplication>

#include "strdecoder.h"
#include "materialFeeder/FeederMgr.h"
#include "common/ModubosProtocol.h"

/******************************************************************************************
* portThread
*******************************************************************************************/
portThread::portThread(QWidget *parent) : QThread(parent)
, m_recdata(new ReceiveData), m_serialport(new QSerialPort)
, m_timer(new QTimer)
{
    connection_state = false;
    buff_state[0] = false;
    buff_state[1] = false;
    m_serialport->moveToThread(this);

    connect(m_timer, &QTimer::timeout, this, &portThread::timer_timeout);
    connect(this, &portThread::timer_stop, this, &portThread::stop_timer);
    connect(m_serialport, &QSerialPort::readyRead, this, &portThread::serial_ready);
    start();
}

portThread::~portThread()
{
    quit();
    m_serialport->deleteLater();
    delete m_recdata;
}

bool portThread::port_setup(const QString &name)
{
    com_name = name;
    if(com_name[0] == '\0')
    {
        return false;
    }
    QList<QSerialPortInfo> com_info = QSerialPortInfo::availablePorts();
    int count = com_info.count();
    if(count <= 0)
    {
       return false;
    }
    for(int i=0;i<count;i++)
    {
        if(com_info.at(i).portName() == com_name)
        {
          break;
        }
        else
        {
          if(i == (count-1))
          {
             return false;
          }
        }
    }

    m_serialport->setPortName(com_name);
    m_serialport->setBaudRate(QSerialPort::Baud115200);
    m_serialport->setStopBits(QSerialPort::OneStop);
    m_serialport->setParity(QSerialPort::NoParity);
    m_serialport->setDataBits(QSerialPort::Data8);

    return true;
}

int  portThread::port_refresh(QVector<QString> *name)
{
    QList<QSerialPortInfo> com_info = QSerialPortInfo::availablePorts();

    int count = com_info.count();
    for(int i=0;i<count;i++)
    {
        name->append(com_info.at(i).portName());
    }
    return count;
}

void portThread::port_open(bool state)
{
     if(state)
     {
       if(m_serialport->open(QSerialPort::ReadWrite) == false)
       {
          m_serialport->close();
       }
     }
     else
     {
       m_serialport->close();
     }
}

bool portThread::port_state()
{
    return m_serialport->isOpen();
}

void portThread::port_write(uint8_t *data, int len)
{
    if(m_serialport->isOpen() == true)
    {
       m_serialport->write((const char*)data,len);
       if(!m_timer->isActive())
           m_timer->start(2000);
    }
    else
    {
       timer_timeout();
    }
}

bool portThread::get_connection_state()
{
    return connection_state;
}

const portThread::TriEleValveStat* portThread::GetTriEleValveStat(int idx) const
{
    if (idx == 0 || idx == 1)
        return m_triEleValveStat + idx;

    return nullptr;
}

float portThread::GetTemperatureOfHeatAndKeep(int idx) const
{
    if (idx > -1 && idx < 4)
        return m_tempHeatAndKeep[idx] * .1;

    return 0;
}

void portThread::stop_timer()
{
    buff_state[0] = buff_state[1];
    buff_state[1] = connection_state;
    if(buff_state[0] != buff_state[1])
    {
        emit port_connected();
    }
    m_timer->stop();
}

void portThread::serial_ready()
{
    if (m_serialport->bytesAvailable() < 1)
        return;

    m_buff += m_serialport->readAll();
    auto msg = pickMsg();
    while (!msg.isEmpty())
    {
        auto data = (uint8_t*)msg.data();
        if (data[2] < 0x80)
            prcsReport(msg);
        else if (data[2] == 0x80 || data[2] == 0x81)
            emit ackRecved(msg);
        else if (data[2] == 0x82)
            prcsHeatAndKeep(msg);
        else if (data[2] == 0x83)
            prcsTriEleValve(msg);

        connection_state = true;
        msg = pickMsg();
    }
}

void portThread::prcsReport(const QByteArray& msg)
{
    auto data = (const uint8_t*)msg.data();
    int index = 4;
    stop_timer();
    switch (data[2])
    {
    case 0:
        data += 4;
        uint16_t tmp;
        for (int i = 0; i < 2; ++i)
        {
            memcpy(&tmp, data, 2); // 反应炉
            data += 2;
            m_recdata->PointTemp[i] = tmp / 10.0;

            m_recdata->ProgStep[i] = *data++;

            memcpy(&tmp, data, 2);
            data += 2;
            m_recdata->ProgTime[i] = tmp;

            memcpy(&tmp, data, 2);
            data += 2;
            m_recdata->Aimtemp[i] = tmp / 10.0;   //目标温度

            m_recdata->mv[i] = *data++;//输出MV

            memcpy(&tmp, data, 2);
            data += 2;
            m_recdata->PointTemp[STOVE_NUM + i] = tmp / 10.0;//温显

            memcpy(&tmp, data, 2);
            data += 2;
            m_recdata->Flow[i] = tmp / 10.0;///流量计

            memcpy(&tmp, data, 2);
            data += 2;
            m_recdata->Flow[FLOW_NUM + i] = tmp / 10.0; ///柱塞流量

            memcpy(&tmp, data, 2);
            data += 2;
            m_recdata->pres[PRESS_NUM + i] = tmp / 10.0; ///柱塞压力

            memcpy(&tmp, data, 2);
            data += 2;
            m_recdata->pres[PRESS_NUM + PUMP_NUM + i] = tmp / 100.0;

            for (int j = i; j < PRESS_NUM; j += 2) ///压力1--6
            {
                memcpy(&tmp, data, 2);
                data += 2;
                m_recdata->pres[j] = tmp / 100.0;
            }
            m_recdata->liquid[i] = *data++;// 展示液位
            data++;
        }
        break;
    case 1:
        for (int i = 0; i < FLOW_NUM; i++)
        {
            m_recdata->Flow_range[i] = (data[index + 2 * i] | (data[index + 2 * i + 1] << 8)) / 10.0;
        }
        index += FLOW_NUM * 2;
        for (int i = 0; i < PRESS_NUM; i++)
        {
            m_recdata->pres_range[i] = (data[index + 2 * i] | (data[index + 2 * i + 1] << 8)) / 10.0;
        }
        index += PRESS_NUM * 2;

        for (int i = 0; i < LIQUID_NUM; i++)
        {
            m_recdata->liquid_range[i] = (data[index + 2 * i] | (data[index + 2 * i + 1] << 8)) / 10.0;
        }
        index += LIQUID_NUM * 2;

        for (int i = 0; i < PUMP_NUM; i++)
        {
            m_recdata->pump_cali[i] = (data[index + 2 * i] | (data[index + 2 * i + 1] << 8)) / 1000.0;
        }
        index += PUMP_NUM * 2;

        for (int i = 0; i < STOVE_NUM; i++)
        {
            m_recdata->adjState[i] = data[index + i];
        }
        index += STOVE_NUM;

        for (int i = 0; i < PUMP_NUM; i++)
        {
            m_recdata->pump_code[i] = data[index + i];
        }
        index += PUMP_NUM;

        for (int i = 0; i < VALVE_NUM; i++)
        {
            m_recdata->valve_code[i] = data[index + i];
        }
        index += VALVE_NUM;

        for (int i = 0; i < SWITCH_NUM; i++)
        {
            m_recdata->Switch_state[i] = data[index + i];
        }
        index += SWITCH_NUM;

        index += 20; //无效数据
        break;
    case 2:
        memcpy(&(m_recdata->collector[0]), &data[index], 50);
        index += 50;
        break;
    default:
        break;
    }

    emit ReceiceDone(m_recdata);
}

void portThread::prcsHeatAndKeep(const QByteArray& msg)
{
    if (msg.at(3) != 12 || msg.size() != 14)
        return;

    for (int i = 0; i < 4; i++)
    {
        uint16_t tmp = uint16_t(msg.at(i*2+4)) | (msg.at(i*2+5)<<8);
        if (tmp != m_tempHeatAndKeep[i])
        {
            m_tempHeatAndKeep[i] = tmp;
            emit heatAndKeepChanged(i, GetTemperatureOfHeatAndKeep(i));
        }
    }
}

void portThread::prcsTriEleValve(const QByteArray& msg)
{
    if (msg.at(3) != 6 || msg.size()!=8)
        return;

    for (int i = 0; i < 2; i++)
    {
        uint8_t tmp = uint8_t(msg.at(i + 4));
        TriEleValveStat st;
        st.pos = tmp & 0x7f;
        st.bReached = (tmp & 0x80) != 0;
        if (m_triEleValveStat[i].pos!=st.pos || m_triEleValveStat[i].bReached!=st.bReached)
        {
            m_triEleValveStat[i] = st;
            emit triEleValveStat(i, m_triEleValveStat[i].pos, m_triEleValveStat[i].bReached);
        }
    }
}

QByteArray portThread::pickMsg()
{
    uint8_t* data = (uint8_t*)m_buff.data();
    auto pos = m_buff.indexOf(0x31);
    while (pos >= 0)
    {
        if (m_buff.size() - pos <3)
            break;

        if (data[pos + 1] == 0x31)
        {
            if (data[pos + 2] == 0x31)
            {
                pos++;
                continue;
            }

            auto len = data[pos+3];
            if (pos + len + 2 > m_buff.size())
                break;

            uint16_t crc = (data[pos + len] << 8) | data[pos + len + 1];
            if (crc == ModubosProtocol::ModbusCrc(data + pos, len))
            {
                auto ret = m_buff.mid(pos, len + 2);
                m_buff = m_buff.mid(pos + len + 2);
                return ret;
            }
        }
        pos = m_buff.indexOf(0x31, pos+1);
    }
    if (pos < 0)
        m_buff.clear();
    else if (pos > 0)
        m_buff = m_buff.mid(pos);

    return QByteArray();
}

void portThread::timer_timeout()
{
     m_timer->stop();
     connection_state = false;
     buff_state[0] = false;
     buff_state[1] = false;
     m_serialport->close();
     emit port_disconnected();
}