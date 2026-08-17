#include "qcirculation.h"
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

QCirculation::QCirculation(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("循环");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QHBoxLayout *Hlayout = new QHBoxLayout;
    CirculList= new QComboBox;
    CirculList->setView(new QListView);  //  必须加入，否则部分样式不生效
    CirculList->addItem("开始循环");
    CirculList->addItem("结束循环");
    CirculList->setFixedSize(180,30);
    CirculList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
                                 "QListView{font-family:SimHei;font-size:20px;}");

    Hlayout->addWidget(CirculList);

    TimesInput = new QLineEdit;
    TimesInput->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5}");
    TimesInput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    TimesInput->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$")));
    TimesInput->setFixedSize(120,30);
    Hlayout->addWidget(TimesInput);

    unit = new QLabel("循环次数");
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

void QCirculation::addBtn_clicked()
{
     if(CirculList->currentText() == "开始循环")
     {
        if(TimesInput->text().toInt() == 0)
        {
            MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
            msg.exec();
            return ;
        }
        emit circulPanelAdd("开始循环 "+TimesInput->text()+" 次");
     }
     else if(CirculList->currentText() == "结束循环")
     {
        emit circulPanelAdd("结束循环");
     }
}
