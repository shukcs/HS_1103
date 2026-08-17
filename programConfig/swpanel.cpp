#include "swpanel.h"
#pragma execution_character_set("utf-8")

SwPanel::SwPanel(QWidget *parent) : QGroupBox(parent)
{
    this->setTitle("阀门");
    this->setStyleSheet("QGroupBox{font-family:SimHei;font-size:22px;}");

    QHBoxLayout *Hlayout = new QHBoxLayout;

    swList = new QComboBox;
    swList->setView(new QListView);  //  必须加入，否则部分样式不生效
    int i;
    for(i=1; i<=6; i++)
    {
       swList->addItem(tr("电磁阀") + QString::number(i));
    }
    const static QString qssCmb = "QComboBox {"
        "border: 1px solid rgb(226,226,226);"
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
        "QListView{font-family:SimHei;font-size:20px;}";
    swList->setFixedSize(180,30);
    swList->setStyleSheet(qssCmb);

    Hlayout->addWidget(swList);

    actionList = new QComboBox;
    actionList->setView(new QListView);  //  必须加入，否则部分样式不生效
    actionList->addItem(tr("开"));
    actionList->addItem(tr("关"));
    actionList->setFixedSize(120,30);
    actionList->setStyleSheet(qssCmb);
    Hlayout->addWidget(actionList);
    Hlayout->addStretch();

    addBtn = new QPushButton;
    addBtn->setFixedSize(28,28);
    addBtn->setIconSize(QSize(25,25));
    addBtn->setIcon(QIcon(":/programConfig/image/add.png"));
    connect(addBtn,SIGNAL(clicked()),this,SLOT(addBtn_clicked()));
    Hlayout->addWidget(addBtn);

    this->setLayout(Hlayout);
}

void SwPanel::addBtn_clicked()
{
    QString str = tr("电磁阀 %1 设置为 %2").arg(swList->currentIndex()).arg(actionList->currentText());
    emit swPanelAdd(str);
}
