#ifndef __ActtionItem_H__
#define __ActtionItem_H__
#include <stdint.h>
#include <stdbool.h>
#include <QString>

class QDataStream;
enum ActionType : int8_t {
    Act_Robot,
    Act_Feeder,
    Act_Servo,
    Act_StepMotor,
    Act_NextWait,

    Act_Delay,      ///延时
    Act_LoopBeg,    ///循环
    Act_LoopEnd,
    Act_CuverRec,///采集记录
	FL_Label,
    Group_PrepareSolidMate,
    Group_StoveFixTube,
    Group_StoveTubeBack,
    Group_AirClear,
    Group_AirIn,
    Group_AirEnd,
    Group_LiquidIn,
    Group_LiquidEnd,
    Group_StoveHeat,
    Act_Cycle,///采集记录
    Act_PreHeat,
    Act_Keep,
};

class ActionAbstrctItem
{
public:
    ActionAbstrctItem(ActionType type, int16_t seq=-1);
    virtual ~ActionAbstrctItem();
    ActionType getType()const;
    uint16_t GetSeq()const;
    void AddToEdit();
    virtual void Save(QDataStream *dstr);
	virtual void Load(QDataStream *dstr);
	virtual void Distribute() = 0;///分发
	virtual QString ToString(bool bStart = true)const = 0;
private:
    friend class ProgmaMgr;
    int16_t    m_seq;
	ActionType	m_type;       ///ActionType
};

class FeederAction : public ActionAbstrctItem
{
public:
	FeederAction(ActionType type, bool bWait = true, int16_t seq = -1);

	bool isWaitFinish()const;
	bool isStart()const;
	void start();

	void Distribute();///分发
private:
	uint8_t m_bWaitFinish;   ///false: 可以同步进行下一个
	uint8_t m_bStart;        ///false: true, 已经开始
};

class StepMotorAction : public FeederAction
{
public:
	StepMotorAction(uint16_t motorType, bool bDir, uint16_t ch = 0, bool bWait=true);

	uint8_t GetMotorType()const;
	uint8_t GetChannel()const;
	bool GetDirector()const;
	void Distribute()override;
    QString ToString(bool b)const override;
protected:
	QString stepMotorActToString() const;
protected:
	uint8_t m_stepType;   ///
	uint8_t m_stepCh : 4;   ///0 or 1
	bool m_stepDirCont : 1; ///true: 打开炉膛 /反应管上升
};

class ServoMotorAction : public FeederAction
{
public:
	ServoMotorAction(uint16_t pos = 0, bool bWait = true);
	QString ToString(bool bStart = true)const override;

	uint8_t GetServoPos()const;
	void Distribute();
protected:
	uint8_t m_servoPose;   ///PCP点位
};

class RobotActionItem : public FeederAction
{
public:
	RobotActionItem(uint16_t step, uint8_t idx=0, bool bWait = true);
	QString ToString(bool bStart = true)const override;

	uint8_t GetStepType()const;
	uint8_t GetIndex()const;
	void Distribute();
protected:
	uint16_t m_robotStep;	///RobotMgr::RobotStep
	uint8_t m_robotIndex;		///料瓶0~~N, 内衬0~~M, 反应管......
};

class DelayItem : public FeederAction
{
public:
	DelayItem(float delay);
	QString ToString(bool bStart = true)const override;

	float GetDelay()const;
	void Distribute();
private:
    float m_fWaitTime;    ///
};

class ActionItem : public FeederAction
{
public:
    struct { ///
        uint16_t cmdFeeder;
        uint8_t cmdAck;
        uint8_t idStore;
        float wFeed;
    };
    ActionItem(uint16_t cmd, int ack = -1, bool bWait = true);
    virtual ~ActionItem();
    void SetFeedCmd(uint16_t cmd, int ack = -1);
    QString ToString(bool bStart = true)const override;
};

#endif // !__ActtionItem_H__
