#ifndef __RobotMgr_H__
#define __RobotMgr_H__

#include <QObject>

class QSerialPort;
class DeviceAct;
class RobotMgr : public QObject
{
    Q_OBJECT
public:
    enum PortStat {
        PortClose,
        NoData,
        Communicate,
    };
    enum RobotAction {
        None = -1,
        Wait = 0,
        OpenDoor = 1,
        CloseDoor = 2,
        MoveTube = 5001,
        Tube2Stove = 3001,  ///反应管入炉膛，家住
        OutStove = 3101,    ///反应管入炉膛机械臂收回
        ClampStoveTube = 4001, ///夹住炉膛中反应管
        StoveTubeOut = 4101,   ///夹出炉膛中反应管
        TubeBack = 6001,
        MoveStore = 7001,
        StoreBack = 8001,
        BottleInBlance =13,     ///支架料瓶进天平
        BottleOutBlance = 14,   ///天平料瓶回支架
        Bottle2Bracket = -1,    ///料瓶到支架
        BottleBack = -1,        ///支架料瓶放回
        Bottle2Tube = 11,
    };
public:
    RobotMgr(QObject *p);
    ~RobotMgr();

    bool IsConnenct()const;
    void ConnectPort();
    QSerialPort *serialPort()const;
    PortStat serialPortStat()const;
public slots:
    void DoAction(const DeviceAct *act);//step RobotAction, idx
protected:
    void timerEvent(QTimerEvent* e)override;

    void ctrl();
    QByteArray pickModbus();
    uint32_t getAckLen(const uint8_t *buf, uint32_t len);
    void onRead();

    bool parse();
    void readStat();
signals:
    void actionDone(int); //RobotStep
    void connectStatChanged(PortStat);
private:
    RobotAction     m_curAct = None;
    int             m_curIdx = -1;
    uint16_t        m_stat = 0;  //0: 没发送，1:发送等待响应，2:响应
    bool            m_bRobotToPos = false;
    bool            m_bPortChaned = false;
    RobotAction     m_actWrite = None;
    int             m_idTimer = -1;
    int64_t         m_lastTmRcv;
    int64_t         m_lastTmCtrl = 0;
    PortStat        m_comStat = PortClose;
    QSerialPort     *m_port=nullptr;
    QByteArray      m_rcvs;
    QString         m_portName;
};

#endif //__RobotMgr_H__
