#include "qfunlabel.h"
#include <QRegularExpressionValidator>
#pragma execution_character_set("utf-8")


QFunLabel::QFunLabel(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("功能标签");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");
    QHBoxLayout *Hlayout = new QHBoxLayout;

    name = new QLineEdit;
    name->setInputMethodHints(Qt::ImhNone);  // 不限制输入
    name->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    name->setFixedSize(180,30);
    name->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout->addWidget(name);
    Hlayout->addStretch();

    addBtn = new QPushButton;
    addBtn->setFixedSize(28,28);
    addBtn->setIconSize(QSize(25,25));
    addBtn->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn,SIGNAL(clicked()),this,SLOT(addBtn_clicked()));
    Hlayout->addWidget(addBtn);

    this->setLayout(Hlayout);
}

void QFunLabel::addBtn_clicked()
{
    if(!name->text().isEmpty())
    {
       emit labelPanelAdd("--- " + name->text() + " ---");
    }
}
