#include "ActionItem.h"
#include <QApplication>
#include "materialFeeder/RobotMgr.h"
#include "materialFeeder/FeederMgr.h"
#include "strDecoder/strDecoder.h"
#pragma execution_character_set("utf-8")

/*
* ActionAbstrctItem
*/
ActionAbstrctItem::ActionAbstrctItem(ActionType t /*= Act_Robot*/, bool bWait, int16_t seq)
: m_seq(seq), m_type(t), m_bStart(false), m_bWaitFinish(bWait), m_bFinish(false)
{
}

ActionAbstrctItem::~ActionAbstrctItem()
{
}

ActionType ActionAbstrctItem::getType() const
{
    return m_type;
}

bool ActionAbstrctItem::isWaitFinish() const
{
    return m_bWaitFinish;
}

bool ActionAbstrctItem::isFinished() const
{
    return m_bFinish;
}

bool ActionAbstrctItem::isStart() const
{
    return m_bStart;
}

void ActionAbstrctItem::start()
{
    m_bStart = true;
}

/*
* ActionItem
*/
ActionItem::ActionItem(ActionType t, bool bWait):ActionAbstrctItem(t, bWait)
{
}

ActionItem::ActionItem(float tmWait) : ActionAbstrctItem(Act_NextWait, true)
, fWaitTime(tmWait)
{
}

ActionItem::~ActionItem()
{
}

void ActionItem::SetFeedCmd(uint16_t cmd, int ack /*= -1*/)
{
    if (getType() == Act_Feeder)
    {
        cmdFeeder = cmd;
        cmdAck = ack < 0 ? cmd + 1 : ack;
    }
}

QString ActionItem::ToString(bool b) const
{
    auto str = b ? QApplication::translate("ActionItem", "开始") : QApplication::translate("ActionItem", "完成");
    switch (getType())
    {
    case Act_Robot:
        return QApplication::translate("ActionItem", "机械臂") + RobotMgr::actionDescribe((RobotMgr::RobotAction)robotStep, robotIndex) + str;
    case Act_Feeder:
        if (!b)
            break;
        if (auto c = cmdFeeder == 104 ? FeederMgr::Instance().GetStore(idStore) : nullptr)
            return  QApplication::translate("ActionItem", "进料器") + FeederMgr::CmdDescrib(cmdFeeder) + (c->pMate ? c->pMate->name : QString());
        return  QApplication::translate("ActionItem", "进料器") + FeederMgr::CmdDescrib(cmdFeeder);
    case Act_Servo:
        return QApplication::translate("ActionItem", "伺服电机执行") + FeederMgr::ServoPosDescrib((FeederMgr::RobotPostion)servoPos) + str;
    case Act_StepMotor:
        return stepMotorActToString() + str;
    case Act_NextWait:
        if (!b)
            break;
        return QApplication::translate("ActionItem", "等待%1秒").arg(fWaitTime);
    default:
        break;
    }
    return QString();
}

QString ActionItem::stepMotorActToString() const
{
    switch ((CtrlType::StepMotorType)stepType)
    {
    case CtrlType::Motor_Tube:   ///反应管上下电机
        return stepDirCont ? QApplication::translate("ActionItem", "反应管上升") : QApplication::translate("ActionItem", "反应管下降");
    case CtrlType::Motor_Stove:  ///炉膛开合电机
        return stepDirCont ? QApplication::translate("ActionItem", "炉膛打开") : QApplication::translate("ActionItem", "炉膛闭合");
    default:
        break;
    }
    return QString();
}