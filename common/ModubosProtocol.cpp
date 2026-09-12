#include "ModubosProtocol.h"
#include <QAbstractSocket>
#include <QSerialPort>
#include <QDateTime>
#include <math.h>

#define WaitTime 450
union DataU
{
    uint8_t cD[4];
    float   fD;
    uint16_t u16D;
    uint32_t u32D;
};

ModubosProtocol::ModubosProtocol(QIODevice *io, uint16_t addr)
: m_lastWrite(0), m_io(io), m_addr(addr)
{
    connect(io, &QIODevice::readyRead, this, &ModubosProtocol::readIO);
    if (dynamic_cast<QSerialPort*>(io))
        m_type = Serial;
    else if (dynamic_cast<QAbstractSocket*>(io))
        m_type = TCP;

    m_idTimer = startTimer(500);
}

bool ModubosProtocol::IsFinishSnd() const
{
    return m_snds.isEmpty();
}

void ModubosProtocol::Send(const void *bf, uint16_t len)
{
    appendSend((const uint8_t *)bf, len);
}

void ModubosProtocol::WriteReg(uint16_t reg, uint16_t val)
{
    uint8_t buf[4];
    AddModbusU16(buf, reg);
    AddModbusU16(buf+2, val);
    appendSend(6, buf, 4);
}

void ModubosProtocol::WriteMultiReg(uint16_t reg, const void *bf, uint16_t len)
{
    QByteArray ar(len + 5, 0);
    AddModbusU16((uint8_t*)ar.data(), reg);
    AddModbusU16((uint8_t*)ar.data()+2, (len+1)/2);
    ar[4] = len;
    ar.replace(5, len, (const char*)bf, len);
    appendSend(0x10, (uint8_t*)ar.data(), ar.size());
}

void ModubosProtocol::ReadReg(uint16_t reg, uint16_t len/*=1*/)
{
    uint8_t buf[4];
    AddModbusU16(buf, reg);
    AddModbusU16(buf + 2, len);
    appendSend(3, buf, 4);
}

void ModubosProtocol::ReadRegOthor(uint16_t reg, uint16_t len /*= 1*/)
{
    uint8_t buf[4];
    AddModbusU16(buf, reg);
    AddModbusU16(buf + 2, len);
    appendSend(4, buf, 4);
}

void ModubosProtocol::Clear()
{
    m_rcv.clear();
    m_snds.clear();
}

QIODevice * ModubosProtocol::GetIO() const
{
    return m_io;
}

uint16_t ModubosProtocol::GetCurCmdAddr() const
{
    if (!m_snds.isEmpty())
    {
        if (TCP == m_type)
            return PichModbusU16(m_snds.first().data() + 8);
        else if (Serial == m_type)
            return PichModbusU16(m_snds.first().data() + 2);
    }

    return 0;
}

bool ModubosProtocol::IsSndAll() const
{
    return m_snds.isEmpty();
}

uint16_t ModubosProtocol::ModbusCrc(const void *bf, uint16_t len)
{
    auto buff = (const uint8_t*)bf;
    unsigned short crc = 0xffff;
    while (len--)
    {
        crc = crc ^ (*buff++);
        for (int i = 0; i < 8; ++i)
        {
            bool b = (crc & 1) == 1;
            crc >>= 1;
            if (b)
                crc ^= 0xA001;
        }
    }

    return (crc >> 8) | (crc << 8);
}

bool ModubosProtocol::Equal(double f1, double f2)
{
    return fabs(f1 - f2) < 0.000001;
}

void ModubosProtocol::AddModbusFloat(uint8_t* buff, float f)
{
    DataU tmp;
    tmp.fD = f;
    buff[0] = tmp.cD[2];
    buff[1] = tmp.cD[3];
    buff[2] = tmp.cD[0];
    buff[3] = tmp.cD[1];
}

void ModubosProtocol::AddModbusU16(uint8_t* buff, uint16_t u)
{
    DataU tmp;
    tmp.u16D = u;
    buff[0] = tmp.cD[1];
    buff[1] = tmp.cD[0];
}

void ModubosProtocol::AddModbusU32(uint8_t* buff, uint32_t u)
{
    DataU tmp;
    tmp.u32D = u;
    buff[0] = tmp.cD[3];
    buff[1] = tmp.cD[2];
    buff[2] = tmp.cD[1];
    buff[3] = tmp.cD[0];
}

float ModubosProtocol::PichModbusFloat(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[2] = buff[0];
    tmp.cD[3] = buff[1];
    tmp.cD[0] = buff[2];
    tmp.cD[1] = buff[3];

    return tmp.fD;
}

uint16_t ModubosProtocol::PichModbusU16(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[0] = buff[1];
    tmp.cD[1] = buff[0];

    return tmp.u16D;
}

uint32_t ModubosProtocol::PichModbusU32(const void* src)
{
    auto buff = (const uint8_t*)src;
    DataU tmp;
    tmp.cD[0] = buff[3];
    tmp.cD[1] = buff[2];
    tmp.cD[2] = buff[1];
    tmp.cD[3] = buff[0];

    return tmp.u32D;
}

void ModubosProtocol::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_idTimer)
    {
        if (!m_snds.isEmpty() && QDateTime::currentMSecsSinceEpoch()-m_lastWrite>=WaitTime)
            write(m_snds.first());
    }
    else
    {
        QObject::timerEvent(e);
    }
}

int ModubosProtocol::getAckLen(uint8_t *buff, uint32_t) const
{
    int ret = -1;
    switch (buff[1])
    {
    case 6:
    case 0x10:
        ret = 8; break;	///回复写数据
    case 3:
    case 4:
        ret = buff[2] + 5; break;
    }
    if (TCP == m_type)
        ret -= 2;

    return ret < 256 ? ret : -1;
}

void ModubosProtocol::appendSend(uint8_t cmd, uint8_t *buff, uint16_t sz)
{
    auto len = sz + (TCP == m_type ? 8 : 4);
    QByteArray arr(len, 0);
    if (TCP == m_type)
    {
        AddModbusU16((uint8_t*)arr.data(), m_seq++);
        AddModbusU32((uint8_t*)arr.data()+2, sz +2);
        *(uint8_t*)(arr.data() + 6) = m_addr;
        *(uint8_t*)(arr.data() + 7) = cmd;
        memcpy(arr.data() + 8, buff, sz);
    }
    else
    {
        *(uint8_t*)(arr.data()) = m_addr;
        *(uint8_t*)(arr.data() + 1) = cmd;
        memcpy(arr.data() + 2, buff, sz);
        AddModbusU16((uint8_t*)arr.data()+2+sz, ModbusCrc(arr.data(), sz+2));
    }
    if (m_snds.isEmpty())
        write(arr);

    m_snds << arr;
}

void ModubosProtocol::appendSend(const uint8_t *buff, uint16_t l)
{
    auto len = l + (TCP == m_type ? 7 : 3);
    QByteArray arr(len, 0);
    if (TCP == m_type)
    {
        AddModbusU16((uint8_t*)arr.data(), m_seq++);
        AddModbusU16((uint8_t*)arr.data() + 2, l + 1);
        *(uint8_t*)(arr.data() + 6) = m_addr;
        memcpy(arr.data() + 7, buff, l);
    }
    else
    {
        *(uint8_t*)(arr.data()) = m_addr;
        memcpy(arr.data() + 1, buff, l);
        AddModbusU16((uint8_t*)arr.data() + 2 + 1, ModbusCrc(arr.data(), l + 1));
    }
    if (m_snds.isEmpty())
        write(arr);

    m_snds << arr;
}

void ModubosProtocol::write(const QByteArray &ar)
{
    if (m_io && m_io->isOpen())
    {
        m_io->write(ar);
        m_lastWrite = QDateTime::currentMSecsSinceEpoch();
    }
}

void ModubosProtocol::readIO()
{
    if (m_io && m_io->bytesAvailable() > 0)
    {
        m_rcv += m_io->readAll();
        pickMsg();
    }
}

void ModubosProtocol::pickMsg()
{
    if (m_rcv.isEmpty())
        return;

    if (Serial == m_type)
    {
        int idx = 0;
        while (1)
        {
            idx = m_rcv.indexOf(char(m_addr), idx);
            if (idx < 0)
            {
                m_rcv.clear();
                return;
            }
            auto remian = m_rcv.size() - idx;
            if (remian < 6)
                break;
            auto len = getAckLen((uint8_t*)m_rcv.data() + idx, m_rcv.size() - idx);
            if (len < 0)
            {
                ++idx;
                continue;
            }

            if (remian < len)
                break;

            auto data = (uint8_t*)m_rcv.data() + idx;
            uint16_t crc = PichModbusU16(data + len - 2);
            if (crc == ModbusCrc(data, len - 2))
            {
                prcsModbus(data + idx + 1, len - 3);
                idx += len;
            }
            else
            {
                idx++;
            }
        }
        if (idx > 0)
            m_rcv.remove(0, idx);
    }
    else if(TCP == m_type)
    {
        int idx = 6;
        while (1)
        {
            idx = m_rcv.indexOf(char(m_addr), idx);
            if (idx < 0)
            {
                if (m_rcv.size() > 6)
                    m_rcv = m_rcv.right(6);
                return;
            }
            auto remian = m_rcv.size() - idx;
            if (remian < 4)
                break;
            auto data = (uint8_t*)m_rcv.data() + idx;
            auto len = getAckLen(data, m_rcv.size() - idx);
            if (remian < len)
                break;

            if (len == PichModbusU32(data - 4))
            {
                prcsModbus(data + 1, len-1);
                idx += len;
            }
            else
            {
                ++idx;
            }
        }
        if (idx > 0)
            m_rcv.remove(0, idx);
    }
}

void ModubosProtocol::prcsModbus(const uint8_t *data, uint16_t len)
{
    if (!m_snds.isEmpty())
    {
        auto cmd = (uint8_t)m_snds.first().at(Serial == m_type ? 1 : 7);
        if (cmd != data[0])
            return;

        emit modbusRcvd(data, len);
        m_snds.removeFirst();
        if (!m_snds.isEmpty())
            write(m_snds.first());
    }
}
