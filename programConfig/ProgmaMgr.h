#ifndef __ProgmaMgr_H__
#define __ProgmaMgr_H__

#include <QObject>
#include <QList>
class ActionAbstrctItem;
class ProgmaMgr : public QObject
{
    Q_OBJECT
public:
	typedef QList<ActionAbstrctItem *> ActionsGroup;
	typedef QPair<QString, ActionsGroup> LoadItem;
	typedef QPair<QString, float> FeedItem;
public:
    ~ProgmaMgr();

	const QList<LoadItem> &AllLoadGroup()const;
    const ActionsGroup &EditActionsGroup()const;
	void Reload(const QString &path);
    void Load(const QString &file);
	bool Save(const QString &file);
	void AddLabel(const QString &label);
	void AddSolidPrepare(uint16_t numBottle, uint16_t numTube, const QList<FeedItem> &weightFeeds);
	void AddFixTube(uint16_t ch, uint16_t numTube);
    void AddTubeRecycle(uint16_t ch, uint16_t pos);
    void AddLiquidClear(uint16_t ch);
    void AddAction(ActionAbstrctItem *act);

    void RemoveAt(int idx);
    void MoveUpAt(int idx);
    void MoveDownAt(int idx);

    static ProgmaMgr &Instance();
protected:
    ProgmaMgr(QObject *p);
    void load(QDataStream *ds, ActionsGroup &f);
    void clearLoads();
    void addItem(ActionAbstrctItem *item);
signals:
    void actionItemAdded(ActionAbstrctItem *);
    void loadsRefrashed();
    void curActionsGroupChanged();
    void itemRemovd(int idx);
    void itemMoveUp(int idx);
    void itemMoveDown(int idx);
protected:
    int m_curGroup = -1;
    ActionsGroup m_edits;
	QList<LoadItem> m_loads;
	static uint16_t s_seq;
};

#endif //__ProgmaMgr_H__