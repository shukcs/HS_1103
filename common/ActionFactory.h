#ifndef __ActionFactory_H__
#define __ActionFactory_H__

#include <map>

class ActionAbstrctItem;
//工厂元素抽象
class ActionAbsFacItem
{
public:
    ActionAbsFacItem(int tp);
    virtual ~ActionAbsFacItem();
    void Register(int tp);
    void Unregister();

    static ActionAbstrctItem *GetActionByType(int tp);
protected:
    virtual ActionAbstrctItem *CreateAction() = 0;
};

template<class Item>
class ActionFacItem : public ActionAbsFacItem
{
public:
    ActionFacItem(int tp) : ActionAbsFacItem(tp)
    {
    }
protected:
    ActionAbstrctItem *CreateAction()
    {
        return new Item();
    }
};

#define DECLARE_ACTIONFACITEM_ITEM(cls, n) ActionFacItem<cls> s_ActionFacItem_##cls(n);

#endif // !__OBJECT_MANAGERS_H__