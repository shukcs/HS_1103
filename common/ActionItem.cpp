#include "ActionItem.h"
#include <QDataStream>
#include <QTimer>
#include "HsApplication.h"
#include "materialFeeder/RobotMgr.h"
#include "materialFeeder/FeederMgr.h"
#include "DevContrlMgr/DevContrlMgr.h"
#include "common/ActionFactory.h"
#include "programConfig/ProgmaMgr.h"
#pragma execution_character_set("utf-8")

/*
* ActionAbstrctItem
*/
ActionAbstrctItem::ActionAbstrctItem(ActionType t /*= Act_Robot*/, int16_t seq)
: m_seq(seq), m_type(t)
{
}

ActionAbstrctItem::~ActionAbstrctItem()
{
}

ActionType ActionAbstrctItem::getType() const
{
    return m_type;
}

uint16_t ActionAbstrctItem::GetSeq() const
{
    return m_seq;
}

void ActionAbstrctItem::AddToEdit()
{
    ProgmaMgr::Instance().AddAction(this);
}

void ActionAbstrctItem::Save(QDataStream *dstr)
{
	if (dstr)
	{
		*dstr << (uint8_t)m_type << m_seq;
	}
}

void ActionAbstrctItem::Load(QDataStream *dstr)
{
    *dstr >> m_seq;
}

/*
* FeederAction
*/
FeederAction::FeederAction(ActionType type, bool bWait, int16_t seq /*= -1*/)
: ActionAbstrctItem(type, seq) , m_bWaitFinish(bWait),m_bStart(false)
{
}

bool FeederAction::isWaitFinish() const
{
    return m_bWaitFinish;
}


bool FeederAction::isStart() const
{
    return m_bStart;
}

void FeederAction::start()
{
	m_bStart = true;
}

void FeederAction::Distribute()
{
}

/*
* ActionItem
*/
StepMotorAction::StepMotorAction(uint16_t motorType, bool bDir, uint16_t ch, bool bWait) : FeederAction(Act_StepMotor, bWait)
, m_stepType(motorType), m_stepCh(ch), m_stepDirCont(bDir)
{
}

QString StepMotorAction::ToString(bool bStart /*= true*/) const
{
	auto str = bStart ? QApplication::translate("ActionItem", "开始") : QApplication::translate("ActionItem", "完成");
	return stepMotorActToString() + str;
}

uint8_t StepMotorAction::GetMotorType() const
{
	return m_stepType;
}

uint8_t StepMotorAction::GetChannel() const
{
	return m_stepCh;
}

bool StepMotorAction::GetDirector() const
{
	return m_stepDirCont;
}

void StepMotorAction::Distribute()
{
	hsApp->devContrlMgr()->onActionRun(this);
}

QString StepMotorAction::stepMotorActToString() const
{
	switch ((CtrlType::StepMotorType)m_stepType)
	{
	case CtrlType::Motor_Tube:   ///反应管上下电机
		return m_stepDirCont ? QApplication::translate("StepMotorAction", "反应管上升") : QApplication::translate("StepMotorAction", "反应管下降");
	case CtrlType::Motor_Stove:  ///炉膛开合电机
		return m_stepDirCont ? QApplication::translate("StepMotorAction", "炉膛打开") : QApplication::translate("StepMotorAction", "炉膛闭合");
	case CtrlType::Motor_Pipelet:  ///炉膛开合电机
		return m_stepDirCont ? QApplication::translate("StepMotorAction", "取液管下降") : QApplication::translate("StepMotorAction", "取液管上升");
	default:
		break;
	}
	return QString();
}

/*
* ServoMotorAction
*/
ServoMotorAction::ServoMotorAction(uint16_t pos /*= 0*/, bool bWait) : FeederAction(Act_Servo, bWait)
, m_servoPose(pos)
{
}

QString ServoMotorAction::ToString(bool bStart /*= true*/) const
{
	auto str = bStart ? QApplication::translate("ServoMotorAction", "开始") : QApplication::translate("ServoMotorAction", "完成");
	return QApplication::translate("ServoMotorAction", "伺服电机执行") + FeederMgr::ServoPosDescrib((FeederMgr::RobotPostion)m_servoPose) + str;
}

uint8_t ServoMotorAction::GetServoPos() const
{
	return m_servoPose;
}

void ServoMotorAction::Distribute()
{
	hsApp->devContrlMgr()->onActionRun(this);
}

/*
* RobotActionItem
*/
RobotActionItem::RobotActionItem(uint16_t step, uint8_t idx/*=0*/, bool bWait /*= true*/) : FeederAction(Act_Robot, bWait)
, m_robotStep(step), m_robotIndex(idx)
{
}

QString RobotActionItem::ToString(bool bStart /*= true*/) const
{
	auto str = bStart ? QApplication::translate("RobotActionItem", "开始") : QApplication::translate("RobotActionItem", "完成");
	return QApplication::translate("RobotActionItem", "机械臂") + RobotMgr::actionDescribe((RobotMgr::RobotAction)m_robotStep, m_robotIndex) + str;
}

uint8_t RobotActionItem::GetStepType() const
{
	return m_robotStep;
}

uint8_t RobotActionItem::GetIndex() const
{
	return m_robotIndex;
}

void RobotActionItem::Distribute()
{
	hsApp->GetRobotMgr()->DoAction(this);
}

/*
* DelayItem
*/
DelayItem::DelayItem(float delay) : FeederAction(Act_NextWait, true)
, m_fWaitTime(delay)
{
}

QString DelayItem::ToString(bool bStart /*= true*/) const
{
	return bStart ? QApplication::translate("DelayItem", "等待%1秒").arg(m_fWaitTime) : QString();
}

float DelayItem::GetDelay() const
{
	return m_fWaitTime;
}

void DelayItem::Distribute()
{
	QTimer::singleShot(m_fWaitTime * 1000, hsApp->feederMgr(), &FeederMgr::OnWait);
}

/*
* ActionItem
*/
ActionItem::ActionItem(uint16_t cmd, int ack, bool bWait):FeederAction(Act_Feeder, bWait)
{
	SetFeedCmd(cmd, ack);
}

ActionItem::~ActionItem()
{
}

void ActionItem::SetFeedCmd(uint16_t cmd, int ack /*= -1*/)
{
	cmdFeeder = cmd;
	cmdAck = ack < 0 ? cmd + 1 : ack;
}

QString ActionItem::ToString(bool b) const
{
	if (b)
	{
		auto str = b ? QApplication::translate("ActionItem", "开始") : QApplication::translate("ActionItem", "完成");
		if (auto c = cmdFeeder == 104 ? hsApp->feederMgr()->GetStore(idStore) : nullptr)
			return  QApplication::translate("ActionItem", "进料器") + FeederMgr::CmdDescrib(cmdFeeder) + (c->pMate ? c->pMate->name : QString());

		return  QApplication::translate("ActionItem", "进料器") + FeederMgr::CmdDescrib(cmdFeeder);
	}
    return QString();
}