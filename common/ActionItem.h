#ifndef __ActtionItem_H__
#define __ActtionItem_H__
#include <stdint.h>
#include <stdbool.h>
#include <QString>

enum ActionType : int8_t {
    Act_Robot,
    Act_Feeder,
    Act_Servo,
    Act_StepMotor,
    Act_NextWait,

    Act_Delay,      ///延时
    Act_LoopBeg,    ///循环
    Act_LoopEnd,
    Act_CuverRecBeg,///采集记录
    Act_CuverRecEnd,

    Group_PrepareSolidMate,
    Group_StoveFixTube,
    Group_StoveTubeBack,
    Group_AirClear,
    Group_AirIn,
    Group_AirEnd,
    Group_LiquiClear,
    Group_LiquiIn,
    Group_LiquiEnd,
    Group_StoveHeat,
};

class ActionAbstrctItem {
public:
    ActionAbstrctItem(ActionType type, bool bWait=true, int16_t seq=-1);
    virtual ~ActionAbstrctItem();
    ActionType getType()const;
    virtual QString ToString(bool bStart = true)const = 0;
    bool isWaitFinish()const;
    bool isFinished()const;
    bool isStart()const;
    void start();
private:
    int16_t    m_seq;
    ActionType	m_type;       ///ActionType
    bool m_bStart : 1;        ///false: true, 已经开始
    bool m_bWaitFinish : 1;   ///false: 可以同步进行下一个
    bool m_bFinish : 1;
};

class ActionItem : public ActionAbstrctItem 
{
public:
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
    ActionItem(ActionType type = Act_Robot, bool bWait = true);
    ActionItem(float tmWait);
    virtual ~ActionItem();
    void SetFeedCmd(uint16_t cmd, int ack = -1);
    QString ToString(bool bStart = true)const override;
private:
    QString stepMotorActToString()const;
};

#endif // !__ActtionItem_H__
