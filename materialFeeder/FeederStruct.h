#ifndef __FEEDERSTRUCT_H__
#define __FEEDERSTRUCT_H__
#include <QList>
#include <QString>

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
    RobotPos_Tube = 1,
    RobotPos_Store = 2,
    RobotPos_Blance = 3,
    RobotPos_Stove = 1,
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

enum DeviceType {
    Dev_Robot,
    Dev_Feeder,
    Dev_Servo,
    Dev_StepMotor,
    Dev_NextWait,
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
    uint8_t type = 0; ///0:固体； 1:液体
    QString name;
    QString nfcid;
    MaterialStruct(const QString &nfc, const QString &name = QString(), float weight = 0.0f);
};

class StoreStruct {
public:
    StoreStruct(int num, const MaterialStruct *m, const QString &id);
    StoreStat getStat()const;
    void setStat(StoreStat, bool bRvr=false);
private:
    StoreStat  stat = S_None;
public:
    int numb;
    const MaterialStruct *pMate;
    QString     nfcid;
};

class BottleStruct
{
public:
    uint16_t m_numb;
    uint16_t m_stWork;
    BottleStruct(uint16_t numb, uint16_t flag = B_CanUse, uint16_t stWork = 0);
    BottleStat getFlag()const;
    void setFlag(BottleStat st, bool bRvr=false);
private:
    uint16_t m_flag;
};

class TubeStruct {
public:
    TubeStruct(uint16_t n, uint16_t flag = T_WaitPrepare);
    TubeStat getFlag()const;
    void setFlag(TubeStat, bool bRvr = false);
    uint16_t getNumber()const;
    int getStoveCh()const;
    void setStoveCh(int8_t ch);
private:
    uint16_t m_numb;
    uint16_t m_flag;
    int16_t  m_chStove = -1;
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
    QString ToString(bool bStart = true)const;
private:
    QString stepMotorActToString()const;
};

class FeederParam
{
public:
    typedef QPair<int, float> FeedItem;
public:
    FeederParam(const QList<FeedItem> &feeds = QList<FeedItem>(), uint16_t numb = 0, uint16_t nTube = 0);
    int16_t getBottleNumb()const;
    BottleStruct *getBottle()const;
    uint16_t getTubeNumb()const;
    TubeStruct *getTube()const;
    const QList<FeedItem> &feedMaterial()const;
    void getFeedNameAndWeight(QList<QPair<QString, float> > *ret)const;
    void feederFinish(int type)const;
private:
    QList<FeedItem>m_feedMaterials;
    uint16_t    m_numbBottle;
    uint16_t     m_numbTube;
};

class QFile;
class FeederRecover {
public:
    enum RvcType{
        R_None,
        R_Bottle = 1,
        R_Tube,
        R_Store,
        R_Param,
        R_Jobs,
    };
    class RecoverItem {
    public:
        uint16_t _len;
        uint16_t _type;      ///RvcType 1:料瓶；2:反应管；3:料仓；4配料参数；5:jobs
        uint16_t _id;        ///识别码
        uint32_t _offset;    ///保存位置
        RecoverItem(uint16_t len=0, RvcType type=R_None, uint32_t id=0, uint32_t offset=6);
    };
public:
    ~FeederRecover();

    void Clear();
    const QList<RecoverItem> &AllRecoverItems()const;
    void FeedJobStart(uint16_t actSz);
    void FeedActionDone();

    void AddBottle(const BottleStruct &bt);
    void RecoverBottle(BottleStruct *bt);
    void AddTube(const TubeStruct &tb);
    void RecoverTube(TubeStruct *bt);
    void AddStore(const StoreStruct &st);
    void RecoverStore(StoreStruct *bt);
    void AddFeedParam(const FeederParam &pr);
    void Removed(const FeederParam &pr);
    void RecoverFeedParam(QList<FeederParam> &feeds);
    void AddJobs(const QList<int> &jobs);
    void RecoverJobs(QList<int> &feeds);
    uint16_t GetRemainActions()const;

    void Save();
    static FeederRecover &Instance();
private:
    FeederRecover(const QString &file);
    bool setSize(uint32_t sz);
    QList<RecoverItem>::iterator getItem(RvcType type, uint32_t id);

    void writeItem(int32_t from);
    uint16_t writeData(const BottleStruct *bt, const RecoverItem &rc);
    uint16_t writeData(const TubeStruct *tb, const RecoverItem &r);
    uint16_t writeData(const StoreStruct *st, const RecoverItem &r);
    uint16_t writeData(const FeederParam *pr, const RecoverItem &r);
    uint16_t writeData(const QList<int> &jobs, const RecoverItem &r);
    uint16_t writeBase(const RecoverItem &r);
private:
    QFile *m_mapFile; //内存映射文件
    uint32_t m_size = 0;
    uint16_t   m_curAct = 0;
    uint8_t *m_buff; //内存映射文件
    QList<RecoverItem> m_items;
};

Q_DECLARE_METATYPE(const StoreStruct*);
Q_DECLARE_METATYPE(BottleStruct*);
Q_DECLARE_METATYPE(TubeStruct*);
Q_DECLARE_METATYPE(const DeviceAct*);
#endif // !__FEEDERSTRUCT_H__