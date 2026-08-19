#include "hslist.h"

#include <QDropEvent>
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#pragma execution_character_set("utf-8")

HsList::HsList(QWidget *parent) : QListWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);  //这禁用tab键和上下方向键并且除去复选框
}

void HsList::dropEvent(QDropEvent *event)
{
}

void HsList::performDrag()
{
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
