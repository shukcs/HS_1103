#ifndef __RobotMgr_H__
#define __RobotMgr_H__

#include <QObject>

class QSerialPort;
class DeviceAct;
class QTcpSocket;
class RobotMgr : public QObject
{
    Q_OBJECT
public:
    enum ProgmaFlag {
        Progma_None = 0,
        Progma_Pause = 6,
        Progma_Contniue = 7,
    };
    enum RobotStat {
        PortClose,
        NoData,
        Communicate,
        PowerOff,
        PowerOn,
        RobotStart,
        ProgmaStart,
        ProgmaPause,
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
    void ConnectSocket(const QString &ip=QString(), uint16_t port=0);
    RobotStat tcpSocketStat()const;
    const QString &GetHost()const;
    uint16_t GetPort()const;
    bool IsProgmaRun()const;
    void SetPause();
public slots:
    void DoAction(const DeviceAct *act);//step RobotAction, idx
protected:
    void timerEvent(QTimerEvent* e)override;

    RobotStat fromRead(uint16_t s);
protected:
    void ctrl();
    void ctrlProgma();
    QByteArray pickTcpModbus();
    int32_t getAckLen(const uint8_t *buf, uint32_t len);
    void onRead();

    void readStat();
    void initRobot();
    void prcsAction(const QByteArray &msg);
    void prcsStat(const QByteArray &msg);
    void writeInit();
signals:
    void actionDone(int); //RobotStep
    void connectStatChanged(RobotStat);
private:
    RobotAction     m_curAct = None;
    int             m_curIdx = -1;
    uint16_t        m_seq = 0;
    bool            m_bWait = false;
    bool            m_bRobotToPos = false;
    RobotAction     m_actWrite = None;
    int             m_idTimer = -1;
    int64_t         m_lastTmRcv;
    int64_t         m_lastTmCtrl = 0;
    RobotStat       m_comStat = PortClose;
    QTcpSocket      *m_socket = nullptr;
    bool            m_bReadRobotStat=false;
    QString         m_ip;
    uint16_t        m_port;
    bool            m_bStart = true;
    uint16_t        m_speed = 25; ///默认运行速度25%
    bool            m_bSetSpeed = false;
    //QSerialPort     *m_port=nullptr;
    QByteArray      m_rcvs;
    ProgmaFlag      m_progmaFlag = Progma_None;
    //QString         m_portName;
};

#endif //__RobotMgr_H__
