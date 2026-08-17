#include "qslopetemp.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QListView>
#include "common/mymessageBox.h"
#pragma execution_character_set("utf-8")

QSlopeTemp::QSlopeTemp(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("斜率升温");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainlayout);

    /* 以下添加第一行几个组件  */
    QWidget *widget1 = new QWidget;
    QHBoxLayout *widget1layout = new QHBoxLayout;
    widget1->setLayout(widget1layout);

    slopeTempList = new QComboBox;
    slopeTempList->setView(new QListView);  //  必须加入，否则部分样式不生效
    slopeTempList->addItem("反应炉1");
    slopeTempList->addItem("反应炉2");
    slopeTempList->setFixedSize(180,30);
    slopeTempList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
    widget1layout->addWidget(slopeTempList);
    widget1layout->addStretch();

    helpBtn = new QPushButton;
    helpBtn->setFixedSize(28,28);
    helpBtn->setIconSize(QSize(25,25));
    helpBtn->setIcon(QIcon(":/programConfig/image/help.png"));
    connect(helpBtn,SIGNAL(clicked()),this,SLOT(helpBtn_clicked()));
    widget1layout->addWidget(helpBtn);

    addBtn = new QPushButton;
    addBtn->setFixedSize(28,28);
    addBtn->setIconSize(QSize(25,25));
    addBtn->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn,SIGNAL(clicked()),this,SLOT(addBtn_clicked()));
    widget1layout->addWidget(addBtn);

    mainlayout->addWidget(widget1);

    /* 以下添加第二行几个组件  */
    QWidget *widget2 = new QWidget;
    QHBoxLayout *widget2layout = new QHBoxLayout;
    widget2->setLayout(widget2layout);

    name[0] = new QLabel("起始温度");
    name[0]->setFixedWidth(180);
    name[0]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    widget2layout->addWidget(name[0]);

    temp1Input = new QLineEdit;
    temp1Input->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    temp1Input->setValidator(new QIntValidator(0, 999, this));
    temp1Input->setFixedWidth(120);
    temp1Input->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    widget2layout->addWidget(temp1Input);

    unit[0] = new QLabel("℃");
    unit[0]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    widget2layout->addWidget(unit[0]);
    widget2layout->addStretch();

    mainlayout->addWidget(widget2);

    /* 以下添加第三行几个组件  */
    QWidget *widget3 = new QWidget;
    QHBoxLayout *widget3layout = new QHBoxLayout;
    widget3->setLayout(widget3layout);

    name[1] = new QLabel("升温时间");
    name[1]->setFixedWidth(180);
    name[1]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    widget3layout->addWidget(name[1]);

    timeInput = new QLineEdit;
    timeInput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    timeInput->setValidator(new QIntValidator(0, 999, this));
    timeInput->setFixedWidth(120);
    timeInput->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    widget3layout->addWidget(timeInput);

    unit[1] = new QLabel("min");
    unit[1]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    widget3layout->addWidget(unit[1]);
    widget3layout->addStretch();

    mainlayout->addWidget(widget3);

    /* 以下添加第四行几个组件  */
    QWidget *widget4 = new QWidget;
    QHBoxLayout *widget4layout = new QHBoxLayout;
    widget4->setLayout(widget4layout);

    name[2] = new QLabel("目标温度");
    name[2]->setFixedWidth(180);
    name[2]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    widget4layout->addWidget(name[2]);

    temp2Input = new QLineEdit;
    temp2Input->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    temp2Input->setValidator(new QIntValidator(0, 999, this));
    temp2Input->setFixedWidth(120);
    temp2Input->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    widget4layout->addWidget(temp2Input);

    unit[2] = new QLabel("℃");
    unit[2]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    widget4layout->addWidget(unit[2]);
    widget4layout->addStretch();

    mainlayout->addWidget(widget4);

}

void QSlopeTemp::helpBtn_clicked()
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("说明");
    dlg->resize(480,150);

    QVBoxLayout *layout = new QVBoxLayout;
    dlg->setLayout(layout);

    layout->addStretch();
    QLabel text1;
    text1.setAlignment(Qt::AlignHCenter);
    text1.setText("输入框数值非空");
    layout->addWidget(&text1);

    QLabel text2;
    text2.setAlignment(Qt::AlignHCenter);
    text2.setText("时间>0，升温；时间=0，降温");
    layout->addWidget(&text2);
    layout->addStretch();

    QWidget w;
    layout->addWidget(&w);

    QHBoxLayout *hlayout = new QHBoxLayout;
    w.setLayout(hlayout);
    hlayout->addStretch();
    QPushButton btn;
    btn.setText("OK");
    btn.setFixedSize(100,30);
    hlayout->addWidget(&btn);

    connect(&btn,&QPushButton::clicked,this,[=]
    {
        dlg->close();
    });

    hlayout->addStretch();

    dlg->exec();
}

void QSlopeTemp::addBtn_clicked()
{
    if(temp1Input->text().isEmpty() || temp2Input->text().isEmpty() || timeInput->text().isEmpty())
    {
        MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }

    if(timeInput->text().toInt() == 0)  //  判断时间是否为0
    {
       emit slopePanelAdd(slopeTempList->currentText()+" "+QString::number(slopeTempList->currentIndex())+" 降温");
    }
    else if(timeInput->text().toInt() > 0)  //  判断时间是否为0
    {
       emit slopePanelAdd(slopeTempList->currentText()+" "+QString::number(slopeTempList->currentIndex())+" "+timeInput->text()+" min内从 "+temp1Input->text()+" ℃"+"升温至 "+temp2Input->text()+" ℃");
    }
}
