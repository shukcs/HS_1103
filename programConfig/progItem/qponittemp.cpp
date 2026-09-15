#include "qponittemp.h"
#include <QRegularExpressionValidator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QListView>
#include "common/mymessageBox.h"
#pragma execution_character_set("utf-8")

QPonitTemp::QPonitTemp(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("单点升温");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QHBoxLayout *Hlayout = new QHBoxLayout;
    pointTempList= new QComboBox;
    pointTempList->setView(new QListView);  //  必须加入，否则部分样式不生效
    pointTempList->addItem("汽化炉");
    pointTempList->setFixedSize(180,30);
    pointTempList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
    Hlayout->addWidget(pointTempList);

    tempInput = new QLineEdit;
    tempInput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tempInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    tempInput->setFixedWidth(120);
    tempInput->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout->addWidget(tempInput);

    unit = new QLabel("℃");
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

void QPonitTemp::addBtn_clicked()
{
    if(tempInput->text().isEmpty())
    {
        MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }
    emit pointTempPanelAdd(pointTempList->currentText()+" "+QString::number(pointTempList->currentIndex())+" 温度设置为 "+tempInput->text()+" ℃");
}
