#include "qvalvectrl.h"
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

QValveCtrl::QValveCtrl(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("背压阀");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QHBoxLayout *Hlayout = new QHBoxLayout;

//    name = new QLabel("压    力");
//    name->setFixedWidth(180);
//    Hlayout->addWidget(name);

    valveList = new QComboBox;
    valveList->setView(new QListView);
//    int i;
//    for(i=1;i<2;i++)
//    {
//       valveList->addItem("背压阀" + QString::number(i));
//    }
    valveList->addItem("背压阀1");
    valveList->addItem("背压阀2");
    valveList->setFixedSize(180,30);
    valveList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
                                 "border-radius: 5px;"
                                 "padding: 1px 18px 1px 3px;"
                                 "background-color: rgb(255,255,255);"
                                 "font-family:SimHei;"
                                 "font-size:20px;}"
                                 "QComboBox::drop-down {subcontrol-origin: padding;"
                                 "subcontrol-position: top right;"
                                 "width: 20px;"
                                 "padding-right:5px;"
                                 "border-left: none;}"
                                 "QComboBox::down-arrow {image: url(:/stateBar/image/arrow.png);"
                                 "width: 20px;"
                                 "height: 20px;}"
                                 "QComboBox QAbstractItemView::item {min-height: 30px;}"
                                 "QListView{font-family:SimHei;font-size:20px;}");
    Hlayout->addWidget(valveList);

    pres = new QLineEdit;
    pres->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pres->setPlaceholderText("设定压力");
//    pres->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    pres->setFixedWidth(120);
    pres->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout->addWidget(pres);

    unit = new QLabel("MPa");
    unit->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout->addWidget(unit);
    Hlayout->addStretch();

    addBtn = new QPushButton;
    addBtn->setFixedSize(28,28);
    addBtn->setIconSize(QSize(25,25));
    addBtn->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn,SIGNAL(clicked()),this,SLOT(addBtn_clicked()));
    Hlayout->addWidget(addBtn);

    this->setLayout(Hlayout);
}

void QValveCtrl::addBtn_clicked()
{
    if(pres->text().isEmpty())
    {
       return;
    }
    emit valvePanelAdd(valveList->currentText()+" "+QString::number(valveList->currentIndex())+" 压力设置为 "+pres->text()+ " MPa");
}
