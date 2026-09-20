#include "ActionFactory.h"
#include <QMap>

QMap<int, ActionAbsFacItem*> sFacItems;
ActionAbsFacItem::ActionAbsFacItem(int tp)
{
    Register(tp);
}

ActionAbsFacItem::~ActionAbsFacItem()
{
    Unregister();
}

void ActionAbsFacItem::Register(int tp)
{
    if (sFacItems.find(tp) == sFacItems.end())
        sFacItems[tp] = this;
}

void ActionAbsFacItem::Unregister()
{
    sFacItems.remove(sFacItems.key(this, -1));
}

ActionAbstrctItem * ActionAbsFacItem::GetActionByType(int tp)
{
    auto itr = sFacItems.find(tp);
    if (itr != sFacItems.end())
        return itr.value()->CreateAction();
    return nullptr;
}
