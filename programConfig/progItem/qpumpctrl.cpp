#include "qpumpctrl.h"
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpressionValidator>
#include "common/mymessageBox.h"
#pragma execution_character_set("utf-8")

QPumpCtrl::QPumpCtrl(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("柱塞泵");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QVBoxLayout *Vlayout = new QVBoxLayout;
    Vlayout->setContentsMargins(0,0,0,0);
    this->setLayout(Vlayout);

    QWidget *w1 = new QWidget;
    QWidget *w2 = new QWidget;
    QWidget *w3 = new QWidget;
    Vlayout->addWidget(w1);
    Vlayout->addWidget(w2);
    Vlayout->addWidget(w3);

    QHBoxLayout *Hlayout = new QHBoxLayout;

    pumpList = new QComboBox;
    pumpList->setView(new QListView);
//    int i;
//    for(i=1;i<2;i++)
//    {
//       pumpList->addItem("柱塞泵" + QString::number(i));
//    }
    pumpList->addItem("柱塞泵1");
    pumpList->addItem("柱塞泵2");
    pumpList->setFixedSize(180,30);
    pumpList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
    Hlayout->addWidget(pumpList);
    Hlayout->addStretch();
    helpBtn = new QPushButton;
    helpBtn->setFixedSize(28,28);
    helpBtn->setIconSize(QSize(25,25));
    helpBtn->setIcon(QIcon(":/programConfig/image/help.png"));
    connect(helpBtn,SIGNAL(clicked()),this,SLOT(helpBtn_clicked()));
    Hlayout->addWidget(helpBtn);
    w1->setLayout(Hlayout);

    QHBoxLayout *Hlayout1 = new QHBoxLayout;
    QLabel* label1 = new QLabel("设置流量");
    label1->setFixedWidth(180);
    label1->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout1->addWidget(label1);

    speed = new QLineEdit;
    speed->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //speed->setPlaceholderText("设定速率");
//   speed->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    speed->setFixedWidth(120);
    speed->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout1->addWidget(speed);

    unit = new QLabel("ml/min");
    unit->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout1->addWidget(unit);
    Hlayout1->addStretch();

    addBtn = new QPushButton;
    addBtn->setFixedSize(28,28);
    addBtn->setIconSize(QSize(25,25));
    addBtn->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn,SIGNAL(clicked()),this,SLOT(addBtn_clicked()));
    Hlayout1->addWidget(addBtn);
    w2->setLayout(Hlayout1);

    QHBoxLayout *Hlayout2 = new QHBoxLayout;
    QLabel* label2 = new QLabel("设置启停");
    label2->setFixedWidth(180);
    label2->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout2->addWidget(label2);

    actList = new QComboBox;
    actList->setView(new QListView);  //  必须加入，否则部分样式不生效
    actList->addItem("关闭");
    actList->addItem("启动");

    actList->setFixedSize(120,30);
    actList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
    Hlayout2->addWidget(actList);
    Hlayout2->addStretch();

    addBtn2 = new QPushButton;
    addBtn2->setFixedSize(28,28);
    addBtn2->setIconSize(QSize(25,25));
    addBtn2->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn2,SIGNAL(clicked()),this,SLOT(addBtn2_clicked()));
    Hlayout2->addWidget(addBtn2);
    w3->setLayout(Hlayout2);
    w3->hide();
}

void QPumpCtrl::helpBtn_clicked()
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
    text2.setText("流量>0，运行；流量=0，停止");
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

void QPumpCtrl::addBtn_clicked()
{
      if(speed->text().isEmpty())
      {
          MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
          msg.exec();
          return;
      }
      emit pumpPanelAdd(pumpList->currentText()+" "+QString::number(pumpList->currentIndex())+" 流量设置为 "+speed->text()+ " ml/min");
}

void QPumpCtrl::addBtn2_clicked()
{
    QString str;
    str = pumpList->currentText()+" "+QString::number(pumpList->currentIndex())+" 运行 " + actList->currentText() +" "+QString::number(actList->currentIndex());

    emit pumpPanelAdd(str);
}
