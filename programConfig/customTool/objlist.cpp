#include "objlist.h"
#pragma execution_character_set("utf-8")

objList::objList(QWidget *parent) : QWidget(parent)
{
    initComponent();
}

objList::~objList()
{
    m_pChatListScrollArea->deleteLater();
}

void objList::initComponent()
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

void objList::add_Btn(QString name,int index)
{
      QPushButton* pushButton = new QPushButton(widget);
      pushButton->setText(name);
      pushButton->setMinimumSize(pushButton->size());
      pushButton->setObjectName(QString::number(index));
      this->m_pSCVLayout->addWidget(pushButton);

      connect(pushButton,&QPushButton::clicked,this,&objList::btn_clicked);
}

void objList::list_clear()
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

void objList::setBtn_Enable(int index)
{
    int i;
    for(i=0;i<m_pSCVLayout->count();i++)
    {
       QPushButton *btn = qobject_cast<QPushButton *> (m_pSCVLayout->itemAt(i)->widget());
       if(i == index)
       {
          btn->setEnabled(true);
          obj_clicked(btn->objectName().toInt());  // 发出信号
       }
       else
       {
          btn->setEnabled(false);
       }
    }

}

void objList::setAllBtn_Enable()
{
    int i;
    for(i=0;i<m_pSCVLayout->count();i++)
    {
       QPushButton *btn = qobject_cast<QPushButton *> (m_pSCVLayout->itemAt(i)->widget());
       btn->setEnabled(true);
    }
}

void objList::btn_clicked()
{
     QPushButton *btn = qobject_cast<QPushButton *>(sender());
     obj_clicked(btn->objectName().toInt());
}
