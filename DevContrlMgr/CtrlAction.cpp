#include "CtrlAction.h"
#include <QApplication>
#include <QDataStream>
#include "common/ActionFactory.h"
#include "HsApplication.h"
#include "DevContrlMgr.h"
#include "customGraph/customgraph.h"
#pragma execution_character_set("utf-8")

/*
* ActionItem
*/
LabelItem::LabelItem(const QString &label) : ActionAbstrctItem(FL_Label)
, m_label(label)
{
}

const QString & LabelItem::Name() const
{
	return m_label;
}

void LabelItem::Save(QDataStream *dstr)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr);
		*dstr << m_label;
	}
}

QString LabelItem::ToString(bool) const
{
	return QString("--- %1 ---").arg(m_label);
}

void LabelItem::Load(QDataStream *dstr)
{
	if (dstr)
	{
		ActionAbstrctItem::Load(dstr);
		*dstr >> m_label;
	}
}

void LabelItem::Distribute()
{
}

/*
*AirClrAction
*/
AirClrAction::AirClrAction(uint16_t ch, float prs, uint16_t sec, int16_t seq) : ActionAbstrctItem(Group_AirClear, seq)
, m_ch(ch), m_tm(sec), m_pressure(prs)
{
}

QString AirClrAction::ToString(bool /*= true*/) const
{
    return QApplication::translate("AirClrAction", "吹扫空气: 通道%1以气压%2MPa吃扫气路%3秒").arg(m_ch+1)
        .arg(m_pressure).arg(m_tm);
}

void AirClrAction::Save(QDataStream *dstr)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr);
        *dstr << m_ch << m_tm << m_pressure;
    }
}

void AirClrAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_ch >> m_tm >> m_pressure;
}

void AirClrAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

uint16_t AirClrAction::GetChannel() const
{
    return m_ch;
}

uint16_t AirClrAction::ClearSeconds() const
{
    return m_tm;
}

float AirClrAction::GetPressure() const
{
    return m_pressure;
}

/*
*AirInAction
*/
AirInAction::AirInAction(uint16_t ch, float prsIn, float mlPmin, float prsKp, int16_t seq)
: ActionAbstrctItem(Group_AirIn, seq), m_ch(ch), m_prsIn(prsIn), m_prsKp(prsKp), m_mlPmin(mlPmin)
{
}

QString AirInAction::ToString(bool) const
{
    return QApplication::translate("AirInAction", "进气开始: 通道%1以气压%2MPa, 流速%3ml/min进气; 背压阀以%4Mpa保持")
        .arg(m_ch+1).arg(m_prsIn).arg(m_mlPmin).arg(m_prsKp);
}

void AirInAction::Save(QDataStream *dstr)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr);
        *dstr << m_ch << m_prsIn << m_prsKp << m_mlPmin;
    }
}

void AirInAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;
    ActionAbstrctItem::Load(dstr);
    *dstr >> m_ch >> m_prsIn >> m_prsKp >> m_mlPmin;
}

void AirInAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

uint16_t AirInAction::GetChannel() const
{
    return m_ch;
}

float AirInAction::GetPressIn() const
{
    return m_prsIn;
}

float AirInAction::GetPressKeep() const
{
    return m_prsKp;
}

float AirInAction::GetFlow() const
{
    return m_mlPmin;
}

/*
*AirEndAction
*/
AirEndAction::AirEndAction(uint16_t ch, int16_t seq) : ActionAbstrctItem(Group_AirEnd, seq), m_ch(ch)
{
}

QString AirEndAction::ToString(bool /*= true*/) const
{
    return QApplication::translate("AirEndAction", "进气结束: 通道%1进气结束").arg(m_ch+1);
}


void AirEndAction::Save(QDataStream *dstr)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr);
        *dstr << m_ch;
    }
}

void AirEndAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_ch;

}

void AirEndAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

uint16_t AirEndAction::GetChannel() const
{
    return m_ch;
}

/*
*LiquidInAction
*/
LiquidInAction::LiquidInAction(uint16_t ch, float mlPmin, uint16_t sec, int16_t seq /*= -1*/)
: ActionAbstrctItem(Group_LiquidIn, seq), m_ch(ch), m_tmAirOut(sec), m_mlPmin(mlPmin)
{
}

QString LiquidInAction::ToString(bool /*= true*/) const
{
    return QApplication::translate("LiquidInAction", "进液开始: 通道%1以%2ml/min流速先排空%3秒, 然后进液")
        .arg(m_ch + 1).arg(m_mlPmin).arg(m_tmAirOut);
}

void LiquidInAction::Save(QDataStream *dstr)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr);
        *dstr << m_ch << m_tmAirOut << m_mlPmin;
    }
}

void LiquidInAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_ch >> m_tmAirOut >> m_mlPmin;
}

void LiquidInAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

uint16_t LiquidInAction::GetChannel() const
{
    return m_ch;
}

uint16_t LiquidInAction::ClearAirSeconds() const
{
    return m_tmAirOut;
}

float LiquidInAction::GetFlow() const
{
    return m_mlPmin;
}

/*
*LiquidEndAction
*/
LiquidEndAction::LiquidEndAction(uint16_t ch /*= 0*/, int16_t seq /*= -1*/)
: ActionAbstrctItem(Group_LiquidEnd, seq), m_ch(ch)
{
}

QString LiquidEndAction::ToString(bool /*= true*/) const
{
    return QApplication::translate("LiquidEndAction", "进液结束: 通道%1进液结束").arg(m_ch + 1);
}

void LiquidEndAction::Save(QDataStream *dstr)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr);
        *dstr << m_ch;
    }
}

void LiquidEndAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_ch;
}

void LiquidEndAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

uint16_t LiquidEndAction::GetChannel() const
{
    return m_ch;
}

/*
*DelayAction
*/
DelayAction::DelayAction(float min/*, uint16_t ch*/, int16_t seq) : ActionAbstrctItem(Act_Delay, seq)
, m_delayMin(min)/*, m_ch(ch)*/
{
}

QString DelayAction::ToString(bool /*= true*/) const
{
	return QApplication::translate("DelayAction", "延时: 延时%1min").arg(m_delayMin);
}

void DelayAction::Save(QDataStream *dstr)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr);
		*dstr << m_delayMin;
	}
}

void DelayAction::Load(QDataStream *dstr)
{
	if (!dstr)
		return;

	ActionAbstrctItem::Load(dstr);
	*dstr >> m_delayMin;
}

void DelayAction::Distribute()
{
}

float DelayAction::DelayMinutes() const
{
    return m_delayMin;
}

float DelayAction::DelaySeconds(uint32_t msec) const
{
    auto ret = int(m_delayMin * 60 + .5);
    if (ret < msec)
        ret = msec;
    return ret;
}

/*
*RecordAction
*/
RecordAction::RecordAction(bool bRec /*= 0*/, int16_t seq /*= -1*/)
: ActionAbstrctItem(Act_CuverRec, seq), m_bRec(bRec)
{
}

QString RecordAction::ToString(bool /*= true*/) const
{
    auto str = m_bRec ? QApplication::translate("DelayAction", "开始") : QApplication::translate("DelayAction", "结束");
    return QApplication::translate("DelayAction", "曲线记录: %1").arg(str);
}

void RecordAction::Save(QDataStream *dstr)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr);
        *dstr << m_bRec;
    }
}

void RecordAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_bRec;
}

void RecordAction::Distribute()
{
    if (m_bRec)
    {
        hsApp->getGustomGraph()->startRecord(true);
    }
    else
    {
        hsApp->getGustomGraph()->startRecord(false);
        hsApp->getGustomGraph()->autoSavedata();
    }
}

bool RecordAction::IsRecord() const
{
    return m_bRec;
}

/*
*RecordAction
*/
CycleAction::CycleAction(uint16_t nCycle /*= 0*/, int16_t seq /*= -1*/)
: ActionAbstrctItem(Act_Cycle, seq), m_nCycle(nCycle)
{
}

QString CycleAction::ToString(bool /*= true*/) const
{
    if (m_nCycle>0)
        return QApplication::translate("CycleAction", "开始循环 %1 次").arg(m_nCycle);
    return QApplication::translate("CycleAction", "结束循环");
}

void CycleAction::Save(QDataStream *dstr)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr);
        *dstr << m_nCycle;
    }
}

void CycleAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_nCycle;
}

void CycleAction::Distribute()
{
}

bool CycleAction::IsEnd() const
{
    return m_nCycle < 1;
}

uint16_t CycleAction::CycleCount() const
{
    return m_nCycle;
}

/*
*HeatAction
*/
HeatAction::HeatAction(uint16_t tmp /*= 0*/, uint16_t ch, int16_t seq /*= -1*/)
: ActionAbstrctItem(Act_PreHeat, seq), m_nTmp(tmp), m_nCh(ch)
{
}

QString HeatAction::ToString(bool bStart /*= true*/) const
{
    auto str =QApplication::translate("HeatAction", "加热套: 通道%1加热温度%2℃").arg(m_nCh+1).arg(m_nTmp);
    return str;
}

void HeatAction::Save(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Save(dstr);
    *dstr << m_nTmp << m_nCh;
}

void HeatAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_nTmp >> m_nCh;
}

void HeatAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

uint16_t HeatAction::GetTemp() const
{
    return m_nTmp;
}

uint16_t HeatAction::GetChannel() const
{
    return m_nCh;
}

/*
*KeepAction
*/
KeepAction::KeepAction(uint16_t tmp /*= 0*/, uint16_t ch, int16_t seq /*= -1*/)
: ActionAbstrctItem(Act_Keep, seq), m_nTmp(tmp), m_nCh(ch)
{
}

QString KeepAction::ToString(bool bStart /*= true*/) const
{
    auto str = QApplication::translate("KeepAction", "保温箱: 通道%1保持温度%2℃").arg(m_nCh+1).arg(m_nTmp);
    return str;
}

void KeepAction::Save(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Save(dstr);
    *dstr << m_nTmp << m_nCh;
}

void KeepAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_nTmp >> m_nCh;
}

void KeepAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

uint16_t KeepAction::GetTemp() const
{
    return m_nTmp;
}

uint16_t KeepAction::GetChannel() const
{
    return m_nCh;
}

/*
*StoveHeatAction
*/
StoveHeatAction::StoveHeatAction(uint8_t ch, float tmpBeg /*= 0*/, float tmpDst /*= 0*/, float min/*=0*/, int16_t seq /*= -1*/)
: ActionAbstrctItem(Group_StoveHeat, seq), m_nCh(ch), m_tmpBeg(tmpBeg), m_tmpDst(tmpDst), m_upMin(min)
{
}

QString StoveHeatAction::ToString(bool bStart /*= true*/) const
{
    if (int(m_upMin *100))
        return QApplication::translate("StoveHeatAction", "斜率升温: 炉膛%1用时%2分钟, 从%3℃升温至%3℃")
            .arg(m_nCh+1).arg(m_upMin).arg(m_tmpBeg).arg(m_tmpDst);

    return QApplication::translate("StoveHeatAction", "斜率升温: 炉膛%1停止升温").arg(m_nCh+1);
}

void StoveHeatAction::Save(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Save(dstr);
    *dstr << m_nCh << m_tmpBeg << m_tmpDst << m_upMin;

}

void StoveHeatAction::Load(QDataStream *dstr)
{
    if (!dstr)
        return;

    ActionAbstrctItem::Load(dstr);
    *dstr >> m_nCh >> m_tmpBeg >> m_tmpDst>>m_upMin;
}

void StoveHeatAction::Distribute()
{
    hsApp->devContrlMgr()->onActionRun(this);
}

float StoveHeatAction::UpMins() const
{
    return m_upMin;
}

float StoveHeatAction::UpSeconds() const
{
    return m_upMin * 60;
}

float StoveHeatAction::GetBegTemperature()const
{
    return m_tmpBeg;
}

float StoveHeatAction::GetDstTemperature()const
{
    return m_tmpDst;
}

uint16_t StoveHeatAction::GetChannel() const
{
    return m_nCh;
}

DECLARE_ACTIONFACITEM_ITEM(LabelItem, FL_Label)
DECLARE_ACTIONFACITEM_ITEM(AirClrAction, Group_AirClear)
DECLARE_ACTIONFACITEM_ITEM(AirInAction, Group_AirIn)
DECLARE_ACTIONFACITEM_ITEM(LiquidInAction, Group_LiquidIn)
DECLARE_ACTIONFACITEM_ITEM(LiquidEndAction, Group_LiquidEnd)
DECLARE_ACTIONFACITEM_ITEM(DelayAction, Act_Delay)
DECLARE_ACTIONFACITEM_ITEM(RecordAction, Act_CuverRec)
DECLARE_ACTIONFACITEM_ITEM(CycleAction, Act_Cycle)
DECLARE_ACTIONFACITEM_ITEM(HeatAction, Act_PreHeat)
DECLARE_ACTIONFACITEM_ITEM(KeepAction, Act_Keep)
DECLARE_ACTIONFACITEM_ITEM(StoveHeatAction, Group_StoveHeat)
