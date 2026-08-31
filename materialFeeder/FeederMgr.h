#ifndef __FeederDecoder_H__
#define __FeederDecoder_H__
#include <QMap>
#include "FeederStruct.h"

class QSerialPort;
class StepMotorStat;
struct WorkItem;
class ModubosProtocol;
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
    void ReuseBottle(int num);
    void ReuseTube(int num);
public:
    static RobotPostion getStovePos(int ch);
    static QString DefaultConfigFile();
    static QString AppDir(const QString& subDir);
    static FeederMgr& Instance();
    static QString CmdDescrib(uint16_t cmd);
    static QString ServoPosDescrib(RobotPostion pos);
public slots:
    void OnRobotDone(int step);
    void OnServoMotor(int pos, bool bReached);
    void OnStepMotor(StepMotorStat *st);
protected:
    void timerEvent(QTimerEvent* event)override;
    void decode(const uint8_t *buff, uint16_t len);
    int indexOfMaterial(const QString& id);
    void addMaterial(const MaterialStruct& m, bool bAdd=true);
    void updateStore(const QString& nfcid, int idx);

    bool prcsRead(uint16_t addr, const uint8_t *buff, uint16_t len);
	bool prcsStat(uint16_t addr, const uint8_t *buff, uint16_t len);
	bool prcsWriteCmd(uint16_t addr);
    bool prcsFeederStat(uint16_t stat);
    void checkMatesCanFeed();
    void sumFeederWeight(QMap<int, float> *feeds) const;

    void onWait();
private:
    void readMaterials(int idx = 0);
    void readStore(int idx = 0);
    void writeMaterial(const MaterialStruct *m, uint16_t idx);
    void writeCmd(const DeviceAct &act);
    void writeFunc(uint16_t cmd, uint16_t val=1);
    void readFeedStat();
    void readFeedWeight();
    bool doAction();
    void genPrepareActions(const WorkItem &item, bool bDo=true);
    void genTubToStvoe(const WorkItem &item, bool bDo = true);
    void genBackActions(const WorkItem &itemb, bool bDo = true);///回收反应管
    void checkActions(bool bDo=true);
	bool addWorkItem(const struct WorkItem &item);
	QList<JobType> tubeJobs(int numTub)const;

    StoreStruct* getStore(const QString& id)const;
	StoreStruct* getStore(uint16_t num)const;
    MaterialStruct* getMaterial(const QString& id)const;
    const FeederParam *getfeedParamsByTube(int numb) const;
    StoreStruct* getPropStore(float weight, const QString& name, const QMap<int, float> &preDistrs)const;
    BottleStruct *getBottle(int numb) const;
    void actionDone(QList<DeviceAct>::iterator itr);

    void addFeederAct(uint16_t cmd, bool bWait = true, int32_t act = -1, uint8_t numStore=0xff, float wFeed=0.0);
    void addServoMotorAct(RobotPostion pos, bool bWait = true);
    void addStepMotorAct(uint8_t type, uint8_t ch, bool bCont, bool bWait = true);
    void adddRobotAct(uint16_t type, uint8_t index = 0, bool bWait = true);
    void recoverActions();
signals:
    void connectStatChanged(PortStat);
    void materialChanged(const QString &, const MaterialStruct&);
    void materialAdded(const MaterialStruct &);
    void materialRemoved(int);
    void storeChanged(const StoreStruct *);
    void bottleChanged(const BottleStruct*);
    void tubeChanged(const TubeStruct*);
    void actionRun(const DeviceAct*);
	void feedJobFinished(uint16_t type, uint16_t ch); ///type: JobType类型
    void canUsedBottleChanged();
    void canUsedTubeChanged();
    void matesCanFeedChanged();
    void feedingChanged(float, bool);  ///bool true:投料重量； false，已投
private:
	friend class FeederParam;
	friend class BottleStruct;
	friend class StoreStruct;
    friend class TubeStruct;
    friend class FeederRecover;
private:
    bool                    m_bOk;
    ModubosProtocol         *m_modbus;
    StoreStruct             *m_feedStore = nullptr;
    bool                    m_bPortChaned = false;
    int                     m_idRead=-1;
    int64_t                 m_lastTmRcv;
    PortStat                m_comStat = PortClose;
    uint16_t                m_flag = 0;
	uint16_t                m_nStoreNum = 12;
    uint16_t                m_nBottle;
    uint16_t                m_nTube;
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

#endif // __FeederDecoder