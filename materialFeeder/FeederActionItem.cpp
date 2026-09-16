#include "FeederActionItem.h"
#include <QApplication>
#pragma execution_character_set("utf-8")
/*
*SolidPrepareItem
*/
SolidPrepareItem::SolidPrepareItem(uint16_t numBottle, uint16_t numTube, const QList<FeederItem> &weightFeeds, int16_t seq)
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
        ret + QApplication::translate("SolidPrepareItem", "%1克%2").arg(itr.second, 0, 'g', 3).arg(itr.first);
        if (++i < m_weightFeeds.size())
            ret += ", ";
    }
    return ret + QApplication::translate("SolidPrepareItem", "入料瓶%1, 再倒入反应管%2").arg(m_numBottle).arg(m_numTube);
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
    return QApplication::translate("FixTubeItem", "装载: 反应管%1装载至炉膛%2").arg(m_numTube).arg(m_ch);
}

/*
*TubeBackItem
*/
TubeBackItem::TubeBackItem(uint16_t ch, uint16_t posRcy, int16_t seq) : ActionAbstrctItem(Group_StoveTubeBack, seq)
, m_ch(ch), m_posRcy(posRcy)
{
}

QString TubeBackItem::ToString(bool) const
{
    return QApplication::translate("TubeBackItem", "回收: 回收炉膛%1中反应管至回收位%2").arg(m_ch).arg(m_posRcy);
}
