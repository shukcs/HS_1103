#include "qtcdpanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QFile>
#include <QListView>
#include "common/mymessageBox.h"
#pragma execution_character_set("utf-8")

QTcdpanel::QTcdpanel(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("TCD");

    QVBoxLayout *mainlayout = new QVBoxLayout;
    this->setLayout(mainlayout);

    QWidget *topWidget = new QWidget;   //  上面用来装温度
    mainlayout->addWidget(topWidget);
    QHBoxLayout *toplayout = new QHBoxLayout;
    topWidget->setLayout(toplayout);
    name[0] = new QLabel;
    name[0]->setText("温    度");
    name[0]->setFixedWidth(180);
    toplayout->addWidget(name[0]);

    tempInput = new QLineEdit;
    tempInput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tempInput->setValidator(new QIntValidator(0, 999, this));
    tempInput->setFixedWidth(120);
    toplayout->addWidget(tempInput);

    unit[0] = new QLabel;
    unit[0]->setText("℃");
    toplayout->addWidget(unit[0]);
    toplayout->addStretch();

    addBtn[0] = new QPushButton;
    addBtn[0]->setFixedSize(28,28);
    addBtn[0]->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn[0],SIGNAL(clicked()),this,SLOT(addBtn1_clicked()));
    toplayout->addWidget(addBtn[0]);

    QWidget *bottomWidget = new QWidget;   //  下面用来装桥流
    mainlayout->addWidget(bottomWidget);
    QHBoxLayout *bottomlayout = new QHBoxLayout;
    bottomWidget->setLayout(bottomlayout);
    name[1] = new QLabel;
    name[1]->setText("桥    流");
    name[1]->setFixedWidth(180);
    bottomlayout->addWidget(name[1]);

    currentInput = new QLineEdit;
    currentInput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    currentInput->setValidator(new QIntValidator(0, 999, this));
    currentInput->setFixedWidth(120);
    bottomlayout->addWidget(currentInput);

    unit[1] = new QLabel;
    unit[1]->setText("mA");
    bottomlayout->addWidget(unit[1]);
    bottomlayout->addStretch();

    addBtn[1] = new QPushButton;
    addBtn[1]->setFixedSize(28,28);
    addBtn[1]->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn[1],SIGNAL(clicked()),this,SLOT(addBtn2_clicked()));
    bottomlayout->addWidget(addBtn[1]);
}

void QTcdpanel::addBtn1_clicked()
{
    if(tempInput->text().isEmpty())
    {
        MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }
    emit tcdPanelAdd("TCD温度 设置为 "+tempInput->text()+" ℃");
}

void QTcdpanel::addBtn2_clicked()
{
    if(currentInput->text().isEmpty())
    {
        MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }
    emit tcdPanelAdd("TCD桥流 设置为 "+currentInput->text()+" mA");
}
