#include "objlist.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QLayoutItem>
#include <QScrollArea>
#include <QScroller>
#include <QTimer>

ObjList::ObjList(QWidget *parent) : QWidget(parent)
{
    initComponent();
}

ObjList::~ObjList()
{
    m_pChatListScrollArea->deleteLater();
}

void ObjList::initComponent()
{
    m_pChatListScrollArea = new QScrollArea(this);
    m_pChatListScrollArea->setStyleSheet("QScrollArea{border:none}");
    m_pChatListScrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_pChatListScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pChatListScrollArea->setWidgetResizable(true);
    m_pChatListScrollArea->setWidgetResizable(true);

    widget = new QWidget(this);
    m_pSCVLayout = new QVBoxLayout(widget);
    m_pSCVLayout->setContentsMargins(0, 0, 0, 0);
    m_pSCVLayout->setSizeConstraint(QVBoxLayout::SetMinAndMaxSize);
    m_pChatListScrollArea->setWidget(widget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(this->m_pChatListScrollArea);
    this->setLayout(mainLayout);

    QScroller::grabGesture(m_pChatListScrollArea->viewport(), QScroller::LeftMouseButtonGesture);
}

void ObjList::add_Btn(QString name,int index)
{
      QPushButton* pushButton = new QPushButton(widget);
      pushButton->setText(name);
      pushButton->setMinimumSize(pushButton->size());
      pushButton->setObjectName(QString::number(index));
      this->m_pSCVLayout->addWidget(pushButton);

      connect(pushButton,&QPushButton::clicked,this,&ObjList::btn_clicked);
}

void ObjList::list_clear()
{
   while (QLayoutItem* item = this->m_pSCVLayout->takeAt(0))
   {
       if (QWidget* widget = item->widget())
           widget->deleteLater();

       if (QSpacerItem* spaerItem = item->spacerItem())
           this->m_pSCVLayout->removeItem(spaerItem);

       delete item;
   }
}
static QString qss1 = "QPushButton {"
    "background-color: rgb(121, 80, 148);"
    "color: rgb(255, 255, 255);"
    "border-radius: 8;"
    "border: 1px solid rgb(154, 127, 167);"
    "font - family:SimHei;"
    "font - size:16px;"
    "}"
    "QPushButton::pressed{"
    "background-color: rgb(120, 127, 167);"
    "}";
static QString qss2 = "QPushButton {"
    "background-color: rgb(121, 80, 148);"
    "color: darkGreen;"
    "border-radius: 8;"
    "border: 1px solid rgb(154, 127, 167);"
    "font - family:SimHei;"
    "font - size:16px;"
    "}";

void ObjList::setRun(int index)
{
    if (m_idxRun == index)
        return;

    for (auto i = 0; i < m_pSCVLayout->count(); i++)
    {
        if (QPushButton *btn = qobject_cast<QPushButton *> (m_pSCVLayout->itemAt(i)->widget()))
        {
            auto idx = btn->objectName().toInt();
            btn->setStyleSheet(idx == index  ? qss2 : qss1);
        }
    }
    m_idxRun = index;
    emit obj_clicked(index);  // 发出信号
}

void ObjList::setAllBtn_Enable()
{
    for(auto itr : m_pSCVLayout->findChildren<QPushButton*>())
    {
       itr->setEnabled(true);
    }
    if (auto btn = m_idxRun >= 0 ? findChild<QPushButton*>(QString::number(m_idxRun)) : nullptr)
        btn->setStyleSheet(qss1);
    m_idxRun = -1;
}

void ObjList::btn_clicked()
{
     QPushButton *btn = qobject_cast<QPushButton *>(sender());
     auto idx = btn->objectName().toInt();
     emit obj_clicked(idx);
     if (m_idxRun >= 0 && idx != m_idxRun)
         QTimer::singleShot(2500, this, [=] {
         if (m_idxRun>=0)
            emit obj_clicked(m_idxRun);
     });
}
