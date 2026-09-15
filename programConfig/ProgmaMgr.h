#ifndef __ProgmaMgr_H__
#define __ProgmaMgr_H__

#include <QObject>
class ActionItem;
class ProgmaMgr : public QObject
{
    //Q_OBJECT
    typedef QList<ActionItem *> ActionsGroup;
public:
    ProgmaMgr(QObject *p);
    ~ProgmaMgr();

    void AddLiquidClear();
protected:
signals:
    //void actionItemAdded(ActionItem *);
    //void loadsRefrashed();
    //void curActionsGroupChanged(int);
protected:
    int m_curGroup = -1;
    ActionsGroup m_edits;
    QList<ActionsGroup> m_loads;
};

#endif //__ProgmaMgr_H__