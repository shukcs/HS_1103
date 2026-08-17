#ifndef __FeederDecoder_H__
#define __FeederDecoder_H__
#include <QObject>
#include <QMap>

enum {
    C_None,
    C_CanUse,
    C_WaitStart,
    C_Using,
    C_Used,
    C_Back,
    C_Error,
};

enum {
    RobotPos_Tube=1,
    RobotPos_Store=2,
    RobotPos_Blance=3,
    RobotPos_Stove =1,
};

enum {
    Flag_CanReadMaterial = 1,
    Flag_DoReadMaterial = 1 << 1,
    Flag_CanReadStore = 1 << 2,
    Flag_StoreNumRead = 1 << 3,
    Flag_ReadyReadStore = Flag_CanReadStore | Flag_StoreNumRead,
    Flag_DoReadStore = 1 << 4,
    Flag_ReadStore = Flag_CanReadStore | Flag_StoreNumRead | Flag_DoReadStore,
};

typedef struct {
    uint16_t day : 5;  // 0~31
    uint16_t month : 4;  // 1~12
    uint16_t year : 7;  // 偏移量 0~127 (代表 2000~2127)
} date_packed_t;

class MaterialStruct {
public:
    float weight; //g
	float usedRate=1.0;///比例系数
    union {
        date_packed_t data;
        uint16_t ch;
    };
    uint8_t type=0; ///0:固体； 1:液体
    QString name;
    QString nfcid;
    MaterialStruct(const QString &nfc, const QString &name=QString(), float weight=0.0f);
};

class StoreStruct {
public:
	int numb;
	const MaterialStruct *pMate;
	QString     nfcid;
	uint16_t    stat = 0; ///0:归为，1:机械臂移动，2: 投料中
	StoreStruct(int num, const MaterialStruct *m, const QString &id);
};

enum DeviceType {
	Dev_Robot,
	Dev_Feeder,
	Dev_Servo,
	Dev_StepMotor,
    Dev_NextWait,
};

class DeviceAct {
public:
    DeviceType	type : 8;     ///DeviceType
    bool bStart : 1;   ///false: 可以同步进行下一个
    bool bWaitFinish : 1;   ///false: 可以同步进行下一个
	union {
		uint32_t data = 0;
		struct {
			uint16_t robotStep;	///RobotMgr::RobotStep
			uint8_t robotIndex;		///料瓶0~~N, 内衬0~~M, 反应管......
		};
		struct {
			uint16_t cmdFeeder;
            uint16_t cmdAck;
		};
		struct {
			uint8_t servoPos;
		};
		struct {
			uint8_t stepType;   ///
			uint8_t stepCh:4;   ///0 or 1
			bool stepDirCont:1; ///true: 打开炉膛 /反应管上升
		};
        float fWaitTime;        ///
	};
    DeviceAct(DeviceType type = Dev_Robot, bool bWait = true);
    DeviceAct(float tmWait);
    void SetFeedCmd(uint16_t cmd, int ack = -1);
};

class BottleStruct
{
public:
	uint16_t m_numb;
	uint16_t m_flag;
	uint16_t m_stWork;
    BottleStruct(uint16_t numb, uint16_t flag = C_CanUse, uint16_t stWork = 0);
};

class TubeStruct {
public:
	uint16_t numb;
	uint16_t flag;
    uint16_t chStove;
    bool     bFeeded = false;
public:
    TubeStruct(uint16_t n, uint16_t flag=C_CanUse, uint16_t ch=0);
};

class FeederBottle
{
public:
    FeederBottle(const QList<QPair<int, float> > &feeds=QList<QPair<int, float> >(), uint16_t numb=0, int16_t nTube = -1);
    int16_t getBottleNumb()const;
    int16_t getTubeNumb()const;
    TubeStruct *getTube()const;
    const QList<QPair<int, float> > &feedMaterial()const;
    void getFeedNameAndWeight(QList<QPair<QString, float> > *ret)const;
    bool isRepeat()const;
    void setRepeat(bool b);
    int16_t getChannel()const;
    void setChannel(int16_t ch);
    void feedFinish()const;
private:
    uint16_t  m_numbBottle;
    QList<QPair<int, float> >m_feedMaterials;
    int16_t m_ch;
    int16_t m_numbTube;
    bool m_bRepeat = false;
};

class QSerialPort;
class StepMotorStat;
class FeederDecoder :  public QObject
{
    Q_OBJECT
public:
    enum PortStat {
        PortClose,
        NoData,
        Communicate,
    };
    enum RobotPostion {
        Pos_None = -1,
        Pos_Tube = 1,
        Pos_Store,
        Pos_InOutBlance,
        Pos_BlanceDoor,
        Pos_Stove1,
        Pos_Stove2 = Pos_None,
        Pos_Home = 1,
    };
public:
    FeederDecoder(QObject* p);
    ~FeederDecoder();

    const QStringList &AllAvalidMaterials()const;
    float MaterialsWeight(const QString& name)const;
    void AddMaterial(const QString& nfcid, const QString& name, float weight);
    void ChangeMaterial(const QString& nfcid, float weight, const QString& name, const QString& idNew=QString());
    const MaterialStruct* GetMaterial(const QString& id)const;
    uint32_t GetStoreNum()const;
    const StoreStruct* GetContainer(const QString& id)const;
    const StoreStruct* GetContainer(uint32_t idx)const;
    QString GetContainerName(uint32_t idx)const;
    QStringList GetStoreNfcId(bool bContainUse = true, const QString &cur=QString());
    const QList<BottleStruct*> &AllBottles()const;
    const QList<TubeStruct*> &AllTubes()const;
    QList<TubeStruct*> ValidTubes()const;
    TubeStruct *ValidTube(int index)const;

    bool FeedSolidMaterial(const QMap<QString, float> &feeds, int numb=-1, int nTub=-1, int ch=0);
    bool StoveTubeBack(int num);
    void CancleFeed(BottleStruct* bt);
    QString GetCurPortName()const;
    int GetCurPortBaut()const;

    QSerialPort *serialPort()const;
    PortStat serialPortStat()const;
    void ConnectPort();

    TubeStruct *GetInSotveTube(uint8_t ch)const;
    TubeStruct *getTube(int idx)const;
    const FeederBottle *getfeedParams(int numb) const;
public:
    static FeederDecoder& Instance();
    static uint16_t Modbus_crc16(const uint8_t* buff, uint16_t Len);
    static bool Equal(double f1, double f2);
    static void AddModbusFloat(uint8_t* buff, float f);
    static void AddModbusData(uint8_t* buff, uint16_t f);
    static float PichModbusFloat(const void* buff);
    static uint16_t PichModbusU16(const void* buff);
    static RobotPostion getStovePos(int ch);
    static QString DefaultConfigFile();
public slots:
    void OnRobotDone(int step);
    void OnServoMotor(int pos, bool bReached);
    void OnStepMotor(StepMotorStat *st);
protected:
    void timerEvent(QTimerEvent* event)override;

    void decode(const QByteArray& msg);
    int send(const QByteArray& arr, bool bWaitWAck=true);
    QByteArray pickMsg();
    void append(uint8_t* buff, uint16_t len);
    int indexOfMaterial(const QString& id);
    void addMaterial(const MaterialStruct& m, bool bAdd=true);
    void updateStore(const QString& nfcid, int idx);

    void prcsRead(uint16_t addr, const QByteArray& msg);
    void prcsStat(uint16_t addr, const QByteArray& msg);
	const FeederBottle* curFeedBootle()const;
    void checkMatesCanFeed();
    void sumFeederWeight(QMap<int, float> *feeds) const;
private:
    int getAckLen(uint8_t* buff, uint32_t len)const;
    void readByets();
    void readMaterials(int idx = 0);
    void readStore(int idx = 0);
    void writeMaterial(const MaterialStruct *m, uint16_t idx);
    void writeCmd(uint16_t cmd, const QString &id=QString(), float w=0.0f);
    void writeFunc(uint16_t cmd, uint16_t val=1);
    void readFeedStat();
    void prcsFeederStat(uint16_t stat);
    void doAction();
    void genActions(FeederBottle &bt);
    void genBackActions(const FeederBottle *fb);///回收反应管
    void checkActions();

    StoreStruct* getContainer(const QString& id)const;
    StoreStruct* getContainer(uint16_t num)const;
    MaterialStruct* getMaterial(const QString& id)const;
    const FeederBottle *getfeedParamsByTube(int numb) const;
    StoreStruct* getPropContainer(float weight, const QString& name, const QMap<int, float> &preDistrs)const;
    BottleStruct *getBottle(int numb) const;
    void onWait();

    void addFeederAct(uint16_t cmd, bool bWait = true, int32_t act = -1);
    void addServoMotorAct(RobotPostion pos, bool bWait = true);
    void addStepMotorAct(uint8_t type, uint8_t ch, bool bCont, bool bWait = true);
    void adddRobotAct(uint16_t type, uint8_t index = 0, bool bWait = true);
signals:
    void serialPortError(bool);
    void connectStatChanged(PortStat);
    void materialChanged(const QString &, const MaterialStruct&);
    void materialAdded(const MaterialStruct &);
    void materialRemoved(int);
    void containerChanged(const StoreStruct *);
    void matesCanFeedChanged();
    void bottleChanged(const BottleStruct*);
    void tubeChanged(const TubeStruct*);
    void actionRun(const DeviceAct*);
	void feedTubeChanged(uint8_t ch, bool bFix); ///ch: 通道；bFix: true=反应管安装， false=反应管取回
private:
    friend class FeederBottle;
    bool                    m_bOk;
    QSerialPort             *m_port;
    bool                    m_bPortChaned = false;
    int                     m_nWrite = -1;
    int                     m_idTimer=-1;
    int                     m_idRead=-1;
    int64_t                 m_lastTmRcv;
    int64_t                 m_lastTmSnd=-1;
    PortStat                m_comStat = PortClose;
    uint32_t                m_flag = 0;
    uint16_t                m_nStoreNum = 12;
    uint16_t                m_nBottle;
    uint16_t                m_nTube;
    QByteArray              m_buff;
    QList<QByteArray>       m_sends;
    QList<MaterialStruct *> m_allMaterials;
    QList<StoreStruct *>    m_allStore;
    QList<BottleStruct*>    m_allBottle;
    QList<TubeStruct*>      m_allTube;
	QStringList             m_canFeedMatesNames;
	QList<DeviceAct>		m_actions;
	QList<QVariant>		    m_feedParams;
    QString                 m_portName;
};

Q_DECLARE_METATYPE(const StoreStruct*);
Q_DECLARE_METATYPE(BottleStruct*);
Q_DECLARE_METATYPE(TubeStruct*);
Q_DECLARE_METATYPE(const DeviceAct*);
Q_DECLARE_METATYPE(FeederBottle);

#endif // __FeederDecoder