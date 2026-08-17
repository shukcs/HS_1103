#include "hslist.h"

#include <QDropEvent>
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#pragma execution_character_set("utf-8")

HsList::HsList(QWidget *parent) : QListWidget(parent)
{
    this->setFocusPolicy(Qt::NoFocus);  //这禁用tab键和上下方向键并且除去复选框

//    this->setAcceptDrops(true);
//    this->setDragEnabled(true);

 //   connect(this,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(DeleteWarning(QListWidgetItem *)));
}

void HsList::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event);
//    QListWidget::dropEvent(event);
//    performDrag();
}

void HsList::performDrag()
{
//    QListWidgetItem *item = this->takeItem(this->currentRow());
//    delete  item;
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
