#include "FeederActionItem.h"
#include <QDataStream>
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
    return QString();
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

/*
*FixTubeItem
*/
FixTubeItem::FixTubeItem(uint16_t ch, uint16_t numTube, int16_t seq) : ActionAbstrctItem(Group_StoveFixTube, true, seq)
, m_ch(ch), m_numTube(numTube)
{
}

QString FixTubeItem::ToString(bool bStart /*= true*/) const
{
    return QString();
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
, m_ch(ch), m_poRcy(pos)
{
}

QString TubeBackItem::ToString(bool) const
{
    return QString();
}

void TubeBackItem::Save(QDataStream *dstr, bool bSaveStat /*= false*/)
{
	if (dstr)
	{
		ActionAbstrctItem::Save(dstr, bSaveStat);
		*dstr << m_ch << m_poRcy;
	}
}
