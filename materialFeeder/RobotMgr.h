#ifndef __RobotMgr_H__
#define __RobotMgr_H__

#include <QObject>

class QSerialPort;
class DeviceAct;
class ModubosProtocol;
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
        RobotSArmed,
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

    static QString actionDescribe(RobotAction t, uint16_t a1=1);
public slots:
    void DoAction(const DeviceAct *act);//step RobotAction, idx
protected:
    void timerEvent(QTimerEvent* e)override;

    RobotStat fromRead(uint16_t s);
    void setRobotStat(RobotStat);
protected:
    void ctrl();
    void ctrlProgma();
    void onRead(const uint8_t *buf, uint16_t len);
    void readStat();
    void initRobot();
    void prcsAction(const uint8_t *buf, uint16_t len);
    void prcsStat(const uint8_t *buf, uint16_t len);
signals:
    void actionDone(int); //RobotStep
    void connectStatChanged(RobotStat);
private:
    RobotAction     m_curAct = None;
    int             m_curIdx = -1;
    uint16_t        m_seq = 0;
    bool            m_bRobotToPos = false;
    RobotAction     m_actWrite = None;
    int             m_idTimer = -1;
    int64_t         m_lastTmRcv;
    int64_t         m_lastTmCtrl = 0;
    RobotStat       m_comStat = PortClose;
    ModubosProtocol *m_modbusTcp;
    bool            m_bReadRobotStat=false;
    QString         m_ip;
    uint16_t        m_port;
    bool            m_bStart = true;
    uint16_t        m_speed = 25; ///默认运行速度25%
    bool            m_bSetSpeed = false;
    QByteArray      m_rcvs;
    ProgmaFlag      m_progmaFlag = Progma_None;
};

#endif //__RobotMgr_H__
