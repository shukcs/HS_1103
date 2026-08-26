#ifndef __ModubosProtocol_H__
#define __ModubosProtocol_H__
#include <QObject>
#include <QList>

class QIODevice;
class ModubosProtocol : public QObject{
    Q_OBJECT
public:
    enum ModubosType {
        Serial,
        TCP,
    };
public:
    explicit ModubosProtocol(QIODevice *io, uint16_t addr);

    bool IsFinishSnd()const;

    void Send(const void *bf, uint16_t len);
    void WriteReg(uint16_t reg, uint16_t val);                      ///功能码6
    void WriteMultiReg(uint16_t reg, const void *bf, uint16_t len); ///功能码0x10
    void ReadReg(uint16_t reg, uint16_t len=1);                     ///功能码3
    void ReadRegOthor(uint16_t reg, uint16_t len = 1);              ///功能码4
    void Clear();
    QIODevice *GetIO()const;
    uint16_t GetCurCmdAddr()const;
    bool IsSndAll()const;
public:
    static uint16_t ModbusCrc(const void *bf, uint16_t len);
    static bool Equal(double f1, double f2);
    static void AddModbusFloat(uint8_t* buff, float f);
    static void AddModbusU16(uint8_t* buff, uint16_t f);
    static void AddModbusU32(uint8_t* buff, uint32_t f);
    static float PichModbusFloat(const void* buff);
    static uint16_t PichModbusU16(const void* buff);
    static uint32_t PichModbusU32(const void* buff);
protected:
    void timerEvent(QTimerEvent *e)override;
private:
    int getAckLen(uint8_t *buff, uint32_t)const;
    void appendSend(uint8_t cmd, uint8_t *buff, uint16_t); ///cmd, 功能码;
    void appendSend(const uint8_t *buff, uint16_t); ///cmd, 功能码;
    void write(const QByteArray &ar);

    void readIO();
    void pickMsg();
    void prcsModbus(const uint8_t *data, uint16_t len);
signals:
    void modbusRcvd(const uint8_t *, uint16_t);
private:
    int64_t m_lastWrite;
    int     m_idTimer;
    uint16_t m_seq = 0; ///tcp序号
    uint8_t m_type;
    uint8_t m_addr;
    QIODevice   *m_io;
    QByteArray  m_rcv;
    QList<QByteArray>  m_snds;
};


#endif  // __ModubosProtocol_H__
