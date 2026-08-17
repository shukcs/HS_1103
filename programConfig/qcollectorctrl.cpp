#include "qcollectorctrl.h"
#include <QRegularExpressionValidator>
#pragma execution_character_set("utf-8")

QCollectorCtrl::QCollectorCtrl(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("收集器");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QVBoxLayout* Vlayout = new QVBoxLayout;
    Vlayout->setContentsMargins(0,0,0,0);
    this->setLayout(Vlayout);

    QWidget *w1 = new QWidget;
    QWidget *w2 = new QWidget;
    QWidget *w4 = new QWidget;
    QWidget *w5 = new QWidget;
    QWidget *w3 = new QWidget;
    Vlayout->addWidget(w1);
    Vlayout->addWidget(w2);
    Vlayout->addWidget(w4);
    Vlayout->addWidget(w5);
    Vlayout->addWidget(w3);

    QHBoxLayout *Hlayout = new QHBoxLayout;

    collList = new QComboBox;
    collList->setView(new QListView);
    collList->addItem("收集器");
    collList->setFixedSize(180,30);
    collList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
    Hlayout->addWidget(collList);
    Hlayout->addStretch();
    helpBtn = new QPushButton;
    helpBtn->setFixedSize(28,28);
    helpBtn->setIconSize(QSize(25,25));
    helpBtn->setIcon(QIcon(":/programConfig/image/help.png"));
    connect(helpBtn,SIGNAL(clicked()),this,SLOT(helpBtn_clicked()));
    Hlayout->addWidget(helpBtn);
    w1->setLayout(Hlayout);

    QHBoxLayout *Hlayout1 = new QHBoxLayout;
    QLabel* label1 = new QLabel("设置采样数");
    label1->setFixedWidth(180);
    label1->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout1->addWidget(label1);

    sample[0] = new QLineEdit;
    sample[0]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    sample[0]->setPlaceholderText("1~32");
//   sample->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    sample[0]->setFixedWidth(120);
    sample[0]->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout1->addWidget(sample[0]);

    unit[0] = new QLabel("瓶");
    unit[0]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout1->addWidget(unit[0]);
    Hlayout1->addStretch();
    w2->setLayout(Hlayout1);

    QHBoxLayout *Hlayout3 = new QHBoxLayout;
    QLabel* label3 = new QLabel("设置采样时间");
    label3->setFixedWidth(180);
    label3->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout3->addWidget(label3);

    sample[1] = new QLineEdit;
    sample[1]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //sample[1]->setPlaceholderText("1~32");
//   sample->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    sample[1]->setFixedWidth(120);
    sample[1]->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout3->addWidget(sample[1]);

    unit[1] = new QLabel("秒");
    unit[1]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout3->addWidget(unit[1]);
    Hlayout3->addStretch();
    w4->setLayout(Hlayout3);

    QHBoxLayout *Hlayout4 = new QHBoxLayout;
    QLabel* label4 = new QLabel("设置间隔时间");
    label4->setFixedWidth(180);
    label4->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout4->addWidget(label4);

    sample[2] = new QLineEdit;
    sample[2]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //sample[2]->setPlaceholderText("1~32");
//   sample->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    sample[2]->setFixedWidth(120);
    sample[2]->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    Hlayout4->addWidget(sample[2]);

    unit[2] = new QLabel("秒");
    unit[2]->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout4->addWidget(unit[2]);
    Hlayout4->addStretch();
    w5->setLayout(Hlayout4);

    QHBoxLayout *Hlayout2 = new QHBoxLayout;
    QLabel* label2 = new QLabel("设置状态");
    label2->setFixedWidth(180);
    label2->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout2->addWidget(label2);

    actList = new QComboBox;
    actList->setView(new QListView);  //  必须加入，否则部分样式不生效
    actList->addItem("开始采样(采样模式)");
    actList->addItem("开始采样(采样+稀释)");
    actList->addItem("暂停采样");
    actList->addItem("继续采样");
    //actList->addItem("继续采样(含参)");
    actList->addItem("停止采样");

    actList->setFixedSize(240,30);
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

    addBtn = new QPushButton;
    addBtn->setFixedSize(28,28);
    addBtn->setIconSize(QSize(25,25));
    addBtn->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn,SIGNAL(clicked()),this,SLOT(addBtn_clicked()));
    Hlayout2->addWidget(addBtn);
    w3->setLayout(Hlayout2);
}

void QCollectorCtrl::helpBtn_clicked()
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("说明");
    dlg->resize(480,150);

    QVBoxLayout *layout = new QVBoxLayout;
    dlg->setLayout(layout);

    layout->addStretch();
    QLabel text1;
    text1.setAlignment(Qt::AlignHCenter);
    text1.setText("采样数1~32，开始采样选定模式，支持暂停/继续采样");
    layout->addWidget(&text1);

    QLabel text2;
    text2.setAlignment(Qt::AlignHCenter);
    text2.setText("必须开始采样，其它状态设置方可生效");
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

void QCollectorCtrl::addBtn_clicked()
{
      if(sample[0]->text().isEmpty() || sample[0]->text().toInt() == 0 || sample[1]->text().isEmpty() || sample[1]->text().toInt() == 0 || sample[2]->text().isEmpty() || sample[2]->text().toInt() == 0)
      {
          MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
          msg.exec();
          return;
      }

      QString str;
      if(actList->currentIndex() == 0)
      {
          if(sample[0]->text().toInt() > 32)
          {
              sample[0]->setText(QString::number(32));
          }
          str = collList->currentText()+" "+QString::number(collList->currentIndex())+" " + actList->currentText() +" "+sample[0]->text() +" "+sample[1]->text() +" "+sample[2]->text();
      }
      else if(actList->currentIndex() == 1)
      {
          if(sample[0]->text().toInt() > 16)
          {
              sample[0]->setText(QString::number(16));
          }
          str = collList->currentText()+" "+QString::number(collList->currentIndex())+" " + actList->currentText() +" "+sample[0]->text() +" "+sample[1]->text() +" "+sample[2]->text();
      }
//      else if(actList->currentIndex() == 4)
//      {
//          str = collList->currentText()+" "+QString::number(collList->currentIndex())+" " + actList->currentText() +" "+sample[0]->text() +" "+sample[1]->text() +" "+sample[2]->text();
//      }
      else
      {
          str = collList->currentText()+" "+QString::number(collList->currentIndex())+" " + actList->currentText();
      }
      emit collPanelAdd(str);
}

