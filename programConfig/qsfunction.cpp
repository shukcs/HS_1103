#include "qsfunction.h"
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
#pragma execution_character_set("utf-8")

QSFunction::QSFunction(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("特殊功能");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainlayout);

    QWidget *w1 = new QWidget;   //  上面用来装曲线记录
    mainlayout->addWidget(w1);

    QHBoxLayout *w1layout = new QHBoxLayout;
    w1->setLayout(w1layout);
    name[0] = new QLabel;
    name[0]->setText("曲线记录");
    name[0]->setFixedWidth(180);
    name[0]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    w1layout->addWidget(name[0]);

    actionList = new QComboBox;
    actionList->setFixedSize(120,30);
    actionList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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

    actionList->setView(new QListView);  //  必须加入，否则部分样式不生效
    actionList->addItem("开始");
    actionList->addItem("停止");
    w1layout->addWidget(actionList);
    w1layout->addStretch();

    addBtn[0] = new QPushButton;
    addBtn[0]->setFixedSize(28,28);
    addBtn[0]->setIconSize(QSize(25,25));
    addBtn[0]->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn[0],SIGNAL(clicked()),this,SLOT(addBtn1_clicked()));
    w1layout->addWidget(addBtn[0]);

    QWidget *w3 = new QWidget;   //  下面用来装数据保存
    mainlayout->addWidget(w3);
    QHBoxLayout *w3layout = new QHBoxLayout;
    w3->setLayout(w3layout);
    name[2] = new QLabel;
    name[2]->setText("曲线保存");
    name[2]->setFixedWidth(180);
    name[2]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    w3layout->addWidget(name[2]);
    w3layout->addStretch();

    addBtn[2] = new QPushButton;
    addBtn[2]->setFixedSize(28,28);
    addBtn[2]->setIconSize(QSize(25,25));
    addBtn[2]->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn[2],SIGNAL(clicked()),this,SLOT(addBtn3_clicked()));
    w3layout->addWidget(addBtn[2]);

//    QWidget *w4 = new QWidget;   //  色谱触发
//    mainlayout->addWidget(w4);
//    QHBoxLayout *w4layout = new QHBoxLayout;
//    w4->setLayout(w4layout);
//    name[3] = new QLabel;
//    name[3]->setText("色谱触发");
//    w4layout->addWidget(name[3]);

//    addBtn[3] = new QPushButton;
//    addBtn[3]->setFixedSize(28,28);
//    addBtn[3]->setIcon(QIcon(":/programConfig/image/add.png"));
//    connect(addBtn[3],SIGNAL(clicked()),this,SLOT(addBtn4_clicked()));
//    w4layout->addWidget(addBtn[3]);

//    QWidget *w5 = new QWidget;   //  质谱触发
//    mainlayout->addWidget(w5);
//    QHBoxLayout *w5layout = new QHBoxLayout;
//    w5->setLayout(w5layout);
//    name[4] = new QLabel;
//    name[4]->setText("质谱触发");
//    w5layout->addWidget(name[4]);

//    addBtn[4] = new QPushButton;
//    addBtn[4]->setFixedSize(28,28);
//    addBtn[4]->setIcon(QIcon(":/programConfig/image/add.png"));
//    connect(addBtn[4],SIGNAL(clicked()),this,SLOT(addBtn5_clicked()));
//    w5layout->addWidget(addBtn[4]);
}

void QSFunction::addBtn1_clicked()
{
    emit functionPanelAdd("曲线记录 "+ actionList->currentText());
}

void QSFunction::addBtn2_clicked()
{
    emit functionPanelAdd("基线调整");
}

void QSFunction::addBtn3_clicked()
{
    emit functionPanelAdd("曲线数据保存");
}

void QSFunction::addBtn4_clicked()
{
    emit functionPanelAdd("色谱触发");
}

void QSFunction::addBtn5_clicked()
{
    emit functionPanelAdd("质谱触发");
}

