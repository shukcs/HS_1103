#include "qtimedelay.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QSpacerItem>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QFile>
#include <QListView>
#include "common/mymessageBox.h"
#pragma execution_character_set("utf-8")


QTimeDelay::QTimeDelay(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("延时");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QHBoxLayout *Hlayout = new QHBoxLayout;

    timeInput = new QLineEdit;
    timeInput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    timeInput->setValidator(new QRegularExpressionValidator(QRegularExpression("^(([0-9]+.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*.[0-9]+)|([0-9]*[1-9][0-9]*))$")));
    timeInput->setFixedWidth(180);
    timeInput->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout->addWidget(timeInput);

    unit = new QLabel("min");
    unit->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout->addWidget(unit);
    Hlayout->addStretch();

    addBtn = new QPushButton;
    addBtn->setFixedSize(28,28);
    addBtn->setIcon(QIcon(":/programConfig/image/add.png"));
    addBtn->setIconSize(QSize(25,25));
    connect(addBtn,SIGNAL(clicked()),this,SLOT(addBtn_clicked()));
    Hlayout->addWidget(addBtn);

    this->setLayout(Hlayout);
}

void QTimeDelay::addBtn_clicked()
{
    if(timeInput->text().toFloat() == 0)
    {
        MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
        msg.exec();
       return ;
    }
    QString str;
    str = "延时 "+timeInput->text()+" min";

    emit delayPanelAdd(str);
}
