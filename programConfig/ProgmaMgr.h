#ifndef __ProgmaMgr_H__
#define __ProgmaMgr_H__

#include <QObject>
#include <QList>
class ActionAbstrctItem;
class ProgmaMgr : public QObject
{
    //Q_OBJECT
	typedef QList<ActionAbstrctItem *> ActionsGroup;
	typedef QPair<QString, ActionsGroup> LoadItem;
	typedef QPair<int, float> FeedItem;
public:
    ProgmaMgr(QObject *p);
    ~ProgmaMgr();

	const QList<LoadItem> &AllLoadGroup()const;
	void Reload();
	bool Save(const QString &file);
	void AddLabel(const QString &label);
	void AddSolidPrepare(uint16_t numBottle, uint16_t numTube, const QList<FeedItem> &weightFeeds);
	void AddFixTube(uint16_t ch, uint16_t numTube);
    void AddLiquidClear(uint16_t ch);
protected:
	ActionAbstrctItem *create(uint8_t type);
signals:
    //void actionItemAdded(ActionItem *);
    //void loadsRefrashed();
    //void curActionsGroupChanged(int);
protected:
    int m_curGroup = -1;
    ActionsGroup m_edits;
	QList<LoadItem> m_loads;
	static uint16_t s_seq;
};

#endif //__ProgmaMgr_H__