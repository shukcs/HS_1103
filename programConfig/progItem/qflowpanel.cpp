#include "qflowpanel.h"
#include <QRegularExpressionValidator>
#pragma execution_character_set("utf-8")

QFlowPanel::QFlowPanel(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("流量计");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QVBoxLayout *Vlayout = new QVBoxLayout;
    Vlayout->setContentsMargins(0,0,0,0);
    this->setLayout(Vlayout);

    QWidget *w1 = new QWidget;
    QWidget *w2 = new QWidget;
    QWidget *w3 = new QWidget;
    Vlayout->addWidget(w1);
    Vlayout->addWidget(w3);
    Vlayout->addWidget(w2);

    QHBoxLayout *Hlayout = new QHBoxLayout;
    flowList = new QComboBox;
    flowList->setView(new QListView);  //  必须加入，否则部分样式不生效
    int i;
    for(i=1;i<3;i++)
    {
       flowList->addItem("流量计" + QString::number(i));
    }
    flowList->setFixedSize(180,30);
    flowList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
    Hlayout->addWidget(flowList);
    Hlayout->addStretch();
    w1->setLayout(Hlayout);

    QHBoxLayout *Hlayout1 = new QHBoxLayout;
    QLabel* label1 = new QLabel("设置流量");
    label1->setFixedWidth(180);
    label1->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout1->addWidget(label1);

    flowInput = new QLineEdit;
    flowInput->setStyleSheet("QLineEdit{background-color:rgb(255,255,255);border: 1px solid rgb(226, 226, 226);border-radius:5;"
                        "font-family:SimHei;font-size:22px;}");
    flowInput->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    flowInput->setValidator(new QRegularExpressionValidator(QRegularExpression("^(([0-9]+.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*.[0-9]+)|([0-9]*[1-9][0-9]*))$")));
    flowInput->setFixedWidth(120);
    Hlayout1->addWidget(flowInput);

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

    w3->setLayout(Hlayout1);


    QHBoxLayout *Hlayout2 = new QHBoxLayout;
    QLabel* label2 = new QLabel("设置短接");
    label2->setFixedWidth(180);
    label2->setStyleSheet("QLabel{font-family:SimHei;font-size:22px;}");
    Hlayout2->addWidget(label2);

    flow_sw_actionList = new QComboBox;
    flow_sw_actionList->setView(new QListView);  //  必须加入，否则部分样式不生效
    flow_sw_actionList->addItem("开启");
    flow_sw_actionList->addItem("关闭");

    flow_sw_actionList->setFixedSize(120,30);
    flow_sw_actionList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
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
    Hlayout2->addWidget(flow_sw_actionList);
    Hlayout2->addStretch();

    addBtn2 = new QPushButton;
    addBtn2->setFixedSize(28,28);
    addBtn2->setIconSize(QSize(25,25));
    addBtn2->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn2,SIGNAL(clicked()),this,SLOT(addBtn2_clicked()));
    Hlayout2->addWidget(addBtn2);

    w2->setLayout(Hlayout2);

}

void QFlowPanel::addBtn_clicked()
{
    if(flowInput->text().isEmpty())
    {
        MyMessageBox msg(MyMessageBox::Critical, "提示", "请输入数值", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }

    QString str;
    str = flowList->currentText()+" "+QString::number(flowList->currentIndex())+" 流量设置为 "+flowInput->text() + " ml/min";

    emit flowPanelAdd(str);
}

void QFlowPanel::addBtn2_clicked()
{
    QString str;
    str = flowList->currentText()+" "+QString::number(flowList->currentIndex())+" 短接 " + flow_sw_actionList->currentText();

    emit flowPanelAdd(str);
}
