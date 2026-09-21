#include "hslist.h"
#include <QDropEvent>
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#include "ProgmaMgr.h"
#include "common/ActionItem.h"
#pragma execution_character_set("utf-8")

HsList::HsList(QWidget *parent) : QListWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);  //这禁用tab键和上下方向键并且除去复选框
    initContent();
}

void HsList::dropEvent(QDropEvent *event)
{
}

void HsList::performDrag()
{
}

void HsList::initContent()
{
    auto mgr = &ProgmaMgr::Instance();
    connect(mgr, &ProgmaMgr::actionItemAdded, this, [=](ActionAbstrctItem *it) {
        addItem(it->ToString());
    });
    connect(mgr, &ProgmaMgr::itemRemovd, this, [=](int idx) {
        if (idx >= 0 && idx < count())
            removeItemWidget(item(idx));
    });
    connect(mgr, &ProgmaMgr::itemMoveUp, this, [=](int idx) {
        if (idx > 0 && idx < count())
        {
            auto item = takeItem(idx);
			insertItem(idx - 1, item);
			setCurrentRow(idx - 1);
        }
    });
    connect(mgr, &ProgmaMgr::itemMoveDown, this, [=](int idx) {
        if (idx >= 0 && idx+1 < count())
        {
            auto item = takeItem(idx);
            insertItem(idx + 1, item);
			setCurrentRow(idx + 1);
        }
    });
    connect(mgr, &ProgmaMgr::curActionsGroupChanged, this, [=]() {
        clear();
        for (auto it : ProgmaMgr::Instance().EditActionsGroup())
        {
            addItem(it->ToString());
        }
    });
}

void HsList::DeleteWarning(QListWidgetItem *item)
{
	switch (QMessageBox::information(this, "提示信息", "删除该项？", QMessageBox::Yes | QMessageBox::No))
	{
	case QMessageBox::Yes:
		delete item;
		break;
	case QMessageBox::No:
		break;
	default:
		break;
	}
}
