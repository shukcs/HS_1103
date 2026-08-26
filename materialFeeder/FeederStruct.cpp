#include "FeederMgr.h"
#include <QApplication>
#include "RobotMgr.h"
#include "strDecoder/strdecoder.h"

/*
* MaterialStruct
*/
MaterialStruct::MaterialStruct(const QString &nfc, const QString &name/*=QString()*/, float w/*=0.0f*/)
: name(name), nfcid(nfc), weight(w)
{
}

/*
* StoreStruct
*/
StoreStruct::StoreStruct(int num, const MaterialStruct *m, const QString &id) : numb(num),
pMate(m), nfcid(id)
{
}

StoreStat StoreStruct::getStat() const
{
	return stat;
}

void StoreStruct::setStat(StoreStat s)
{
	if (s != stat)
	{
		stat = s;
		emit FeederMgr::Instance().storeChanged(this);
	}
}
/*
* BottleStruct
*/
BottleStruct::BottleStruct(uint16_t num, uint16_t flag, uint16_t stWork) : m_numb(num)
, m_flag(flag), m_stWork(stWork)
{
}

BottleStat BottleStruct::getFlag() const
{
    return (BottleStat)m_flag;
}

void BottleStruct::setFlag(BottleStat f)
{
    if (f != m_flag)
    {
        bool b = B_CanUse == m_flag || B_CanUse == f;
        m_flag = f;
        emit FeederMgr::Instance().bottleChanged(this);
        if (b)
            emit FeederMgr::Instance().canUsedTubeChanged();
    }
}

/*
* FeederParam
*/
FeederParam::FeederParam(const QList<QPair<int, float> > &feeds, uint16_t numb, int16_t nTube) : m_numbBottle(numb)
, m_feedMaterials (feeds), m_numbTube(nTube)
{
}

int16_t FeederParam::getBottleNumb() const
{
	return m_numbBottle;
}

BottleStruct *FeederParam::getBottle() const
{
    return FeederMgr::Instance().getBottle(m_numbBottle);
}

int16_t FeederParam::getTubeNumb() const
{
    return m_numbTube;
}

TubeStruct *FeederParam::getTube() const
{
    return FeederMgr::Instance().getTube(m_numbTube);
}

const QList<QPair<int, float> > & FeederParam::feedMaterial() const
{
    return m_feedMaterials;
}

void FeederParam::getFeedNameAndWeight(QList<QPair<QString, float> > *ret) const
{
    if (!ret)
        return;

    auto &feeder = FeederMgr::Instance();
    ret->clear();
    for (auto &itr : m_feedMaterials)
    {
        auto c = feeder.GetStore(itr.first);
        if (auto m = c ? c->pMate : nullptr)
            *ret << QPair<QString, float>(m->name, itr.second);
    }
}

void FeederParam::feederFinish(int type) const
{
    auto &feeder = FeederMgr::Instance();
    if (auto tb = feeder.getTube(m_numbTube))
    {
		TubeStat st = T_Prepared;
		switch (type)
		{
		case FeederMgr::J_StoveFixTube:
			st = T_Fixed; break;
		case FeederMgr::J_StoveTubeBack:
			st = T_Recyced; break;
		default:
			break;
		}
        tb->setFlag(st);
        if (auto bt = FeederMgr::J_PrepareMate==type ? getBottle() : nullptr)
            bt->setFlag(B_Used);
    }
}

/*
* FeederParam
*/
TubeStruct::TubeStruct(uint16_t n, uint16_t f)
: m_numb(n), m_flag(f)
{
}

TubeStat TubeStruct::getFlag() const
{
    return (TubeStat)m_flag;
}

void TubeStruct::setFlag(TubeStat f)
{
    if (m_flag != f)
    {
        bool b = B_CanUse == m_flag || B_CanUse == f;
        m_flag = f;
        emit FeederMgr::Instance().tubeChanged(this);

        if (b)
            emit FeederMgr::Instance().canUsedTubeChanged();
    }
    m_flag = f;
}

uint16_t TubeStruct::getNumber() const
{
    return m_numb;
}

int TubeStruct::getStoveCh() const
{
    return m_chStove;
}

void TubeStruct::setStoveCh(int8_t ch)
{
    m_chStove = ch;
}

/*
* DeviceAct
*/
DeviceAct::DeviceAct(DeviceType t, bool bWait) : type(t), bStart(false), bWaitFinish(bWait)
{
}

DeviceAct::DeviceAct(float tmWait) : type(Dev_NextWait), fWaitTime(tmWait), bStart(false), bWaitFinish(false)
{
}

void DeviceAct::SetFeedCmd(uint16_t cmd, int ack /*= -1*/)
{
    if (type == Dev_Feeder)
    {
        cmdFeeder = cmd;
        cmdAck = ack < 0 ? cmd + 1 : ack;
    }
}

QString DeviceAct::ToString(bool b) const
{
    auto str = b ? QApplication::translate("DeviceAct", "开始") : QApplication::translate("DeviceAct", "完成");
    switch (type)
    {
    case Dev_Robot:
        return QApplication::translate("DeviceAct", "机械臂") + RobotMgr::actionDescribe((RobotMgr::RobotAction)robotStep, robotIndex) + str;
    case Dev_Feeder:
        if (!b)
            break;
        if (auto c = cmdFeeder == 104 ? FeederMgr::Instance().GetStore(idStore) : nullptr)
            return  QApplication::translate("DeviceAct", "进料器") + FeederMgr::CmdDescrib(cmdFeeder) + (c->pMate ? c->pMate->name : QString());
        return  QApplication::translate("DeviceAct", "进料器") + FeederMgr::CmdDescrib(cmdFeeder);
    case Dev_Servo:
        return QApplication::translate("DeviceAct", "伺服电机执行")+ FeederMgr::ServoPosDescrib((FeederMgr::RobotPostion)servoPos)+str;
    case Dev_StepMotor:
        return stepMotorActToString() + str;
    case Dev_NextWait:
        if (!b)
            break;
        return QApplication::translate("DeviceAct", "等待%1秒").arg(fWaitTime);
    default:
        break;
    }
    return QString();
}

QString DeviceAct::stepMotorActToString() const
{
    switch ((CtrlType::StepMotorType)stepType)
    {
    case CtrlType::Motor_Tube:   ///反应管上下电机
        return stepDirCont ? QApplication::translate("DeviceAct", "反应管上升") : QApplication::translate("DeviceAct", "反应管下降");
    case CtrlType::Motor_Stove:  ///炉膛开合电机
        return stepDirCont ? QApplication::translate("DeviceAct", "炉膛打开") : QApplication::translate("DeviceAct", "炉膛闭合");
    default:
        break;
    }
    return QString();
}