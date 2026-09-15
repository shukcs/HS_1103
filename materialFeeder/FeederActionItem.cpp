#include "FeederActionItem.h"
#pragma execution_character_set("utf-8")
/*
*SolidPrepareItem
*/
SolidPrepareItem::SolidPrepareItem(uint16_t numBottle, uint16_t numTube, const QMap<QString, float> &weightFeeds, int16_t seq)
: ActionAbstrctItem(Group_PrepareSolidMate, seq), m_numBottle(numBottle), m_numTube(numTube), m_weightFeeds(weightFeeds)
{
}

QString SolidPrepareItem::ToString(bool bStart /*= true*/) const
{
    return QString();
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

/*
*TubeBackItem
*/
TubeBackItem::TubeBackItem(int16_t seq) : ActionAbstrctItem(Group_StoveTubeBack, seq)
{
}

QString TubeBackItem::ToString(bool) const
{
    return QString();
}
