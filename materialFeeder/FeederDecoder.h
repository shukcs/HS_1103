#ifndef __FeederDecoder_H__
#define __FeederDecoder_H__
#include <QObject>
#include <QMap>

enum StoreStat {
	S_None,			///无料仓
	S_NoMate,		///无配料
	S_CanFeed,		///已配料
	S_WaitFeed,
	S_Feeding,
	S_Feeded,
};
enum TubeStat {
	T_None,
	T_WaitPrepare,
	T_Preparing,
	T_Prepared,
	T_WaitFix,
	T_Fixing,
	T_Fixed,
	T_WaitRecycle,
	T_Recycling,
	T_Recyced,
};
enum BottleStat {
	B_None,
	B_CanUse,
	B_WaitStart,
	B_Using,
	B_Used,
	B_Error,
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
    float usedRate = 1.0;///比例系数
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
	StoreStruct(int num, const MaterialStruct *m, const QString &id);
	StoreStat getStat()const;
	void setStat(StoreStat);
private:
	StoreStat  stat = S_None;
public:
	int numb;
	const MaterialStruct *pMate;
	QString     nfcid;
};

enum DeviceType {
	Dev_Robot,
	Dev_Feeder,
	Dev_Servo,
	Dev_StepMotor,
    Dev_NextWait,
};

class BottleStruct
{
public:
	uint16_t m_numb;
	uint16_t m_stWork;
    BottleStruct(uint16_t numb, uint16_t flag = B_CanUse, uint16_t stWork = 0);
	BottleStat getFlag()const;
    void setFlag(BottleStat);
private:
    uint16_t m_flag;
};

class TubeStruct {
public:
    TubeStruct(uint16_t n, uint16_t flag=T_WaitPrepare);
	TubeStat getFlag()const;
    void setFlag(TubeStat);
    uint16_t getNumber()const;
    int getStoveCh()const;
    void setStoveCh(int8_t ch);
private:
    uint16_t m_numb;
    uint16_t m_flag;
    int16_t  m_chStove=-1;
};

class FeederParam
{
public:
    FeederParam(const QList<QPair<int, float> > &feeds = QList<QPair<int, float> >(), uint16_t numb = 0, int16_t nTube = -1);
    int16_t getBottleNumb()const;
    BottleStruct *getBottle()const;
    int16_t getTubeNumb()const;
    TubeStruct *getTube()const;
    const QList<QPair<int, float> > &feedMaterial()const;
    void getFeedNameAndWeight(QList<QPair<QString, float> > *ret)const;
    void feederFinish(int type)const;
private:
    QList<QPair<int, float> >m_feedMaterials;
    uint16_t    m_numbBottle;
    int16_t     m_numbTube;
};

class DeviceAct {
public:
    DeviceType	type : 8;     ///DeviceType
    bool bStart : 1;        ///false: true, 已经开始
    bool bWaitFinish : 1;   ///false: 可以同步进行下一个
    union {
        struct {
            uint16_t robotStep;	///RobotMgr::RobotStep
            uint8_t robotIndex;		///料瓶0~~N, 内衬0~~M, 反应管......
        };
        struct { ///
            uint16_t cmdFeeder;
            uint8_t cmdAck;
            uint8_t idStore;
            float wFeed;
        };
        struct {
            uint8_t servoPos;
        };
        struct {
            uint8_t stepType;   ///
            uint8_t stepCh : 4;   ///0 or 1
            bool stepDirCont : 1; ///true: 打开炉膛 /反应管上升
        };
        struct {
            float fWaitTime;    ///
        };
    };
    DeviceAct(DeviceType type = Dev_Robot, bool bWait = true);
    DeviceAct(float tmWait);
    void SetFeedCmd(uint16_t cmd, int ack = -1);
};

class QSerialPort;
class StepMotorStat;
struct WorkItem;
class FeederMgr :  public QObject
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
        Pos_Home = 0,
    };
    enum JobType {
        J_PrepareMate,    ///反应管备料
        J_StoveFixTube,   ///反应管入炉膛
        J_StoveTubeBack,  ///回收炉膛反应管
        J_Unknow = -1,
    };
public:
    FeederMgr(QObject* p);
    ~FeederMgr();

    const QStringList &AllAvalidMaterials()const;
    float MaterialsWeight(const QString& name)const;
    void AddMaterial(const QString& nfcid, const QString& name, float weight);
    void ChangeMaterial(const QString& nfcid, float weight, const QString& name, const QString& idNew=QString());
    const MaterialStruct* GetMaterial(const QString& id)const;
    uint32_t GetStoreNum()const;
    const StoreStruct* GetStore(const QString& id)const;
    const StoreStruct* GetStore(uint32_t idx)const;
    QStringList GetStoreNfcId(bool bContainUse = true, const QString &cur=QString());
    const QList<BottleStruct*> &AllBottles()const;
    const QList<TubeStruct*> &AllTubes()const;
    QList<TubeStruct*> ValidTubes(JobType t=J_PrepareMate)const;
    QList<BottleStruct*> ValidBottls()const;
    TubeStruct *ValidTube(int index)const;

    bool FeedSolidMaterial(const QMap<QString, float> &feeds, int numb, int nTub=-1, bool bFix=true, int ch=0);
    bool FixTube(uint16_t nTb, uint16_t ch);
    bool StoveTubeBack(int ch, int nBack=-1);
    void CancleFeed(BottleStruct* bt);
    QString GetCurPortName()const;
    int GetCurPortBaut()const;

    QSerialPort *serialPort()const;
    PortStat serialPortStat()const;
    void ConnectPort();

    TubeStruct *GetInSotveTube(uint8_t ch)const;
    TubeStruct *getTube(int idx)const;
	const FeederParam *GetfeedParamsByBottleNum(int numb) const;
	bool CanAddWork(JobType t, int numTub, bool bProg = false)const;
public:
    static FeederMgr& Instance();
    static uint16_t Modbus_crc16(const uint8_t* buff, uint16_t Len);
    static bool Equal(double f1, double f2);
    static void AddModbusFloat(uint8_t* buff, float f);
    static void AddModbusData(uint8_t* buff, uint16_t f);
    static void AddModbusU32(uint8_t* buff, uint32_t f);
    static float PichModbusFloat(const void* buff);
    static uint16_t PichModbusU16(const void* buff);
    static uint32_t PichModbusU32(const void* buff);
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

    bool prcsRead(uint16_t addr, const QByteArray& msg);
	bool prcsStat(uint16_t addr, const QByteArray& msg);
	bool prcsWriteCmd(uint16_t addr, uint16_t cmd);
    bool prcsFeederStat(uint16_t stat);
    void checkMatesCanFeed();
    void sumFeederWeight(QMap<int, float> *feeds) const;
private:
    int getAckLen(uint8_t* buff, uint32_t len)const;
    void readByets();
    void readMaterials(int idx = 0);
    void readStore(int idx = 0);
    void writeMaterial(const MaterialStruct *m, uint16_t idx);
    void writeCmd(const DeviceAct &act);
    void writeFunc(uint16_t cmd, uint16_t val=1);
    void readFeedStat();
    void readFeedWeight();
    bool doAction();
    void genPrepareActions(const WorkItem &item);
    void genTubToStvoe(const WorkItem &item);
    void genBackActions(const WorkItem &itemb);///回收反应管
    void checkActions();
	bool addWorkItem(const struct WorkItem &item);
	QList<JobType> tubeJobs(int numTub)const;

    StoreStruct* getStore(const QString& id)const;
	StoreStruct* getStore(uint16_t num)const;
    MaterialStruct* getMaterial(const QString& id)const;
    const FeederParam *getfeedParamsByTube(int numb) const;
    StoreStruct* getPropStore(float weight, const QString& name, const QMap<int, float> &preDistrs)const;
    BottleStruct *getBottle(int numb) const;

    void addFeederAct(uint16_t cmd, bool bWait = true, int32_t act = -1, uint8_t numStore=0xff, float wFeed=0.0);
    void addServoMotorAct(RobotPostion pos, bool bWait = true);
    void addStepMotorAct(uint8_t type, uint8_t ch, bool bCont, bool bWait = true);
    void adddRobotAct(uint16_t type, uint8_t index = 0, bool bWait = true);
	void onWait();
signals:
    void serialPortError(bool);
    void connectStatChanged(PortStat);
    void materialChanged(const QString &, const MaterialStruct&);
    void materialAdded(const MaterialStruct &);
    void materialRemoved(int);
    void storeChanged(const StoreStruct *);
    void bottleChanged(const BottleStruct*);
    void tubeChanged(const TubeStruct*);
    void actionRun(const DeviceAct*);
	void feedTubeChanged(uint16_t type, uint16_t ch); ///type: JobType类型
    void canUsedBottleChanged();
    void canUsedTubeChanged();
    void matesCanFeedChanged();
    void feedingChanged(float, bool);  ///bool true:投料重量； false，已投
private:
	friend class FeederParam;
	friend class BottleStruct;
	friend class StoreStruct;
    friend class TubeStruct;
private:
    bool                    m_bOk;
    QSerialPort             *m_port;
    StoreStruct             *m_feedStore = nullptr;
    bool                    m_bPortChaned = false;
    int                     m_idTimer=-1;
    int                     m_idRead=-1;
    int64_t                 m_lastTmRcv;
    int64_t                 m_lastTmSnd=-1;
    PortStat                m_comStat = PortClose;
    uint16_t                m_flag = 0;
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
    QList<FeederParam>		        m_feedParams;
    QList<int>                      m_jobs;
    QList<DeviceAct>                m_actions;     ///工作列表
    QString                         m_portName;
};

Q_DECLARE_METATYPE(const StoreStruct*);
Q_DECLARE_METATYPE(BottleStruct*);
Q_DECLARE_METATYPE(TubeStruct*);
Q_DECLARE_METATYPE(const DeviceAct*);
Q_DECLARE_METATYPE(FeederParam);

#endif // __FeederDecoder