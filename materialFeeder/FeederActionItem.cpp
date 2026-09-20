#include "FeederActionItem.h"
#include <QDataStream>
#include <QApplication>
#include "common/ActionFactory.h"
#pragma execution_character_set("utf-8")
/*
*SolidPrepareItem
*/
SolidPrepareItem::SolidPrepareItem(uint16_t numBottle, uint16_t numTube, const QList<FeedItem> &weightFeeds, int16_t seq)
: ActionAbstrctItem(Group_PrepareSolidMate, seq), m_numBottle(numBottle), m_numTube(numTube), m_weightFeeds(weightFeeds)
{
}

QString SolidPrepareItem::ToString(bool bStart /*= true*/) const
{
    if (m_weightFeeds.isEmpty())
        return QString();
    auto ret = QApplication::translate("SolidPrepareItem", "固体配料: 先称取");
    int i = 0;
    for (auto &itr : m_weightFeeds)
    {
        ret += QApplication::translate("SolidPrepareItem", "%1克%2").arg(itr.second, 0, 'g', 3).arg(itr.first);
        if (++i < m_weightFeeds.size())
            ret += ", ";
    }
    return ret + QApplication::translate("SolidPrepareItem", "入料瓶%1; 再倒入反应管%2").arg(m_numBottle+1).arg(m_numTube+1);
}

void SolidPrepareItem::Save(QDataStream *dstr, bool bSaveStat /*= false*/)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr, bSaveStat);
		*dstr << m_numBottle << m_numTube;
		*dstr << m_weightFeeds.size();
		for (auto &itr : m_weightFeeds)
		{
			*dstr << itr.first << itr.second;
		}
	}
}

void SolidPrepareItem::Load(QDataStream *dstr)
{
    if (!dstr)
        return;
    ActionAbstrctItem::Load(dstr);
    qsizetype sz;
    *dstr >> m_numBottle >> m_numTube >>sz;
    for (int i = 0; i < sz; ++i)
    {
        FeedItem item;
        *dstr >> item.first >> item.second;
        m_weightFeeds << item;
    }
}

/*
*FixTubeItem
*/
FixTubeItem::FixTubeItem(uint16_t ch, uint16_t numTube, int16_t seq) : ActionAbstrctItem(Group_StoveFixTube, true, seq)
, m_ch(ch), m_numTube(numTube)
{
}

QString FixTubeItem::ToString(bool bStart /*= true*/) const
{
    return QApplication::translate("FixTubeItem", "装载: 反应管%1装载至炉膛%2").arg(m_numTube+1).arg(m_ch+1);
}

void FixTubeItem::Save(QDataStream *dstr, bool bSaveStat /*= false*/)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr, bSaveStat);
		*dstr << m_ch << m_numTube;
	}
}

/*
*TubeBackItem
*/
TubeBackItem::TubeBackItem(uint16_t ch, uint16_t pos, int16_t seq) : ActionAbstrctItem(Group_StoveTubeBack, seq)
, m_ch(ch), m_posRcy(pos)
{
}

QString TubeBackItem::ToString(bool) const
{
    return QApplication::translate("TubeBackItem", "回收: 回收炉膛%1中反应管至回收位%2").arg(m_ch+1).arg(m_posRcy+1);
}

void TubeBackItem::Save(QDataStream *dstr, bool bSaveStat /*= false*/)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr, bSaveStat);
		*dstr << m_ch << m_posRcy;
	}
}


DECLARE_ACTIONFACITEM_ITEM(SolidPrepareItem, Group_PrepareSolidMate)
DECLARE_ACTIONFACITEM_ITEM(FixTubeItem, Group_StoveFixTube)
DECLARE_ACTIONFACITEM_ITEM(TubeBackItem, Group_StoveTubeBack)