#ifndef __FeederActionItem_H__
#define __FeederActionItem_H__
#include "common/ActionItem.h"
#include <QList>

class SolidPrepareItem :  public ActionAbstrctItem
{
	typedef QPair<int, float> FeedItem;
public:
    SolidPrepareItem(uint16_t numBottle=0, uint16_t numTube=0, const QList<FeedItem>&weightFeeds=QList<FeedItem>(), int16_t seq=-1);
    QString ToString(bool bStart = true)const override;
	void Save(QDataStream *dstr, bool bSaveStat = false)override;
public:
    uint16_t m_numBottle;
    uint16_t m_numTube;
	QList<FeedItem> m_weightFeeds;
};

class FixTubeItem : public ActionAbstrctItem
{
public:
    FixTubeItem(uint16_t ch=0, uint16_t numTub=0, int16_t seq=-1);
	QString ToString(bool bStart = true)const override;
	void Save(QDataStream *dstr, bool bSaveStat = false)override;
public:
    uint16_t m_ch;
    uint16_t m_numTube;
};

class TubeBackItem : public ActionAbstrctItem
{
public:
    TubeBackItem(uint16_t ch=0, uint16_t pos=0, int16_t seq = -1);
    QString ToString(bool bStart = true)const override;
	void Save(QDataStream *dstr, bool bSaveStat = false)override;
public:
    uint16_t m_ch;
    uint16_t m_poRcy;
};
#endif // __FeederDecoder