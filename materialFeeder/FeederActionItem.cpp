#include "FeederActionItem.h"
#include <QDataStream>
#include "HsApplication.h"
#include "common/ActionFactory.h"
#include "FeederMgr.h"
#pragma execution_character_set("utf-8")
/*
*SolidPrepareItem
*/
SolidPrepareItem::SolidPrepareItem(uint16_t numBottle, uint16_t numTube, const QList<FeedItem> &weightFeeds, int16_t seq)
: ActionAbstrctItem(Group_PrepareSolidMate, seq), m_numBottle(numBottle), m_numTube(numTube), m_weightFeeds(weightFeeds)
{
}

QString SolidPrepareItem::ToString(bool /*= true*/) const
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

void SolidPrepareItem::Save(QDataStream *dstr)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr);
		*dstr << m_numBottle << m_numTube;
		*dstr << (int32_t)m_weightFeeds.size();
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
    int32_t sz;
    *dstr >> m_numBottle >> m_numTube >>sz;
    for (int i = 0; i < sz; ++i)
    {
        FeedItem item;
        *dstr >> item.first >> item.second;
        m_weightFeeds << item;
    }
}

void SolidPrepareItem::Distribute()
{
	hsApp->feederMgr()->FeedSolidMaterial(m_weightFeeds, m_numBottle, m_numTube, GetSeq());
}

/*
*FixTubeItem
*/
FixTubeItem::FixTubeItem(uint16_t ch, uint16_t numTube, int16_t seq) : ActionAbstrctItem(Group_StoveFixTube, seq)
, m_ch(ch), m_numTube(numTube)
{
}

QString FixTubeItem::ToString(bool) const
{
    return QApplication::translate("FixTubeItem", "装载: 反应管%1装载至炉膛%2").arg(m_numTube+1).arg(m_ch+1);
}

void FixTubeItem::Save(QDataStream *dstr)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr);
		*dstr << m_ch << m_numTube;
	}
}

void FixTubeItem::Distribute()
{
	hsApp->feederMgr()->FixTube(m_numTube, m_ch, GetSeq());
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

void TubeBackItem::Save(QDataStream *dstr)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr);
		*dstr << m_ch << m_posRcy;
	}
}

void TubeBackItem::Distribute()
{
	hsApp->feederMgr()->StoveTubeBack(m_ch, m_posRcy, GetSeq());
}

DECLARE_ACTIONFACITEM_ITEM(SolidPrepareItem, Group_PrepareSolidMate)
DECLARE_ACTIONFACITEM_ITEM(FixTubeItem, Group_StoveFixTube)
DECLARE_ACTIONFACITEM_ITEM(TubeBackItem, Group_StoveTubeBack)