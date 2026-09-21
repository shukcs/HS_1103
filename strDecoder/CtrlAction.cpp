#include "CtrlAction.h"
#include <QApplication>
#include <QDataStream>

#include "common/ActionFactory.h"
#pragma execution_character_set("utf-8")


AirClrAction::AirClrAction(uint16_t ch, float prs, uint16_t sec, int16_t seq) : ActionAbstrctItem(Group_AirClear, true, seq)
, m_ch(ch), m_tm(sec), m_pressure(prs)
{
}

QString AirClrAction::ToString(bool /*= true*/) const
{
    return QApplication::translate("AirClrAction", "吹扫空气: 通道%1以气压%2MPa吃扫气路%3秒").arg(m_ch+1)
        .arg(m_pressure).arg(m_tm);
}

void AirClrAction::Save(QDataStream *dstr, bool bSaveStat/*=false*/)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr, bSaveStat);
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
DECLARE_ACTIONFACITEM_ITEM(AirClrAction, Group_AirClear)

AirInAction::AirInAction(uint16_t ch, float prsIn, float mlPmin, float prsKp, int16_t seq)
: ActionAbstrctItem(Group_AirIn, true, seq), m_ch(ch), m_prsIn(prsIn), m_prsKp(prsKp), m_mlPmin(mlPmin)
{
}

QString AirInAction::ToString(bool bStart) const
{
    return QApplication::translate("AirInAction", "进气开始: 通道%1以气压%2MPa, 流速%3ml/min进气; 背压阀以%4Mpa保持")
        .arg(m_ch+1).arg(m_prsIn).arg(m_mlPmin).arg(m_prsKp);
}

void AirInAction::Save(QDataStream *dstr, bool bSaveStat/*=false*/)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr, bSaveStat);
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
DECLARE_ACTIONFACITEM_ITEM(AirInAction, Group_AirIn)

AirEndAction::AirEndAction(uint16_t ch, int16_t seq) : ActionAbstrctItem(Group_AirEnd, true, seq), m_ch(ch)
{
}

QString AirEndAction::ToString(bool bStart /*= true*/) const
{
    return QApplication::translate("AirEndAction", "进气结束: 通道%1进气结束").arg(m_ch+1);
}


void AirEndAction::Save(QDataStream *dstr, bool bSaveStat /*= false*/)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr, bSaveStat);
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
DECLARE_ACTIONFACITEM_ITEM(AirEndAction, Group_AirEnd)

LiquidInAction::LiquidInAction(uint16_t ch, float mlPmin, uint16_t sec, int16_t seq /*= -1*/)
: ActionAbstrctItem(Group_LiquiIn, true, seq), m_ch(ch), m_tmAirOut(sec), m_mlPmin(mlPmin)
{
}

QString LiquidInAction::ToString(bool bStart /*= true*/) const
{
    return QApplication::translate("LiquidInAction", "进液开始: 通道%1以%2ml/min流速先排空%3秒, 然后进液").arg(m_ch + 1);
}

void LiquidInAction::Save(QDataStream *dstr, bool bSaveStat /*= false*/)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr, bSaveStat);
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
DECLARE_ACTIONFACITEM_ITEM(LiquidInAction, Group_LiquiIn)


LiquidEndAction::LiquidEndAction(uint16_t ch /*= 0*/, int16_t seq /*= -1*/)
: ActionAbstrctItem(Group_LiquiEnd, true, seq), m_ch(ch)
{
}

QString LiquidEndAction::ToString(bool bStart /*= true*/) const
{
    return QApplication::translate("LiquidEndAction", "进液结束: 通道%1进液结束").arg(m_ch + 1);
}

void LiquidEndAction::Save(QDataStream *dstr, bool bSaveStat /*= false*/)
{
    if (dstr)
    {
        ActionAbstrctItem::Save(dstr, bSaveStat);
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
DECLARE_ACTIONFACITEM_ITEM(LiquidEndAction, Group_LiquiEnd)