#ifndef __FeederActionItem_H__
#define __FeederActionItem_H__
#include "common/ActionItem.h"
#include <QList>

class SolidPrepareItem :  public ActionAbstrctItem
{
public:
    typedef QPair<QString, float> FeederItem;
public:
    SolidPrepareItem(uint16_t numBottle, uint16_t numTube, const QList<FeederItem> &weightFeeds, int16_t seq);
    QString ToString(bool bStart = true)const override;
public:
    uint16_t m_numBottle;
    uint16_t m_numTube;
    QList<FeederItem> m_weightFeeds;
};

class FixTubeItem : public ActionAbstrctItem
{
public:
    FixTubeItem(uint16_t ch, uint16_t numTube, int16_t seq=-1);
    QString ToString(bool bStart = true)const override;
public:
    uint16_t m_ch;
    uint16_t m_numTube;
};

class TubeBackItem : public ActionAbstrctItem
{
public:
    TubeBackItem(uint16_t ch, uint16_t posRcy, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;
public:
    uint16_t m_ch;
    uint16_t m_posRcy;
};
#endif // __FeederDecoder