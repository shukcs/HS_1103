#include "progconfig.h"
#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QListWidgetItem>
#include <QScrollArea>
#include <QFileDialog>
#include <QCoreApplication>
#include <QDialog>
#include <QScroller>
#include <math.h>

#include "hslist.h"
#include "swpanel.h"
#include "qflowpanel.h"
#include "qtimedelay.h"
#include "qcirculation.h"
#include "qponittemp.h"
#include "qslopetemp.h"
#include "qtcdpanel.h"
#include "QSFunction.h"
#include "qfunlabel.h"
#include "qpumpctrl.h"
#include "qvalvectrl.h"
#include "qcollectorctrl.h"
#include "MotorGroupBox.h"
#include "HeatGroupBox.h"
#include "FeedLiquidGroupBox.h"
#include "FeederGroupBox.h"
#include "AirWayGroupBox.h"

#include "ui_progconfig.h"
#pragma execution_character_set("utf-8")

progConfig::progConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::progConfig)
{
    ui->setupUi(this);

    QFunLabel *funName = new QFunLabel;
    connect(funName, &QFunLabel::labelPanelAdd,this, &progConfig::listAdd);
    ui->configLayout->addWidget(funName);

    auto fg = new FeederGroupBox();
    connect(fg, &FeederGroupBox::sig_Add, this, &progConfig::listAdd);
    ui->configLayout->addWidget(fg);

    auto liquid = new FeedLiquidGroupBox();
    connect(liquid, &FeedLiquidGroupBox::sig_Add, this, &progConfig::listAdd);
	ui->configLayout->addWidget(liquid);

	auto air = new AirWayGroupBox();
	connect(air, &AirWayGroupBox::sig_Add, this, &progConfig::listAdd);
	ui->configLayout->addWidget(air);

    SwPanel *sw = new SwPanel;
    connect(sw, &SwPanel::swPanelAdd, this, &progConfig::listAdd);
    ui->configLayout->addWidget(sw);

/*    auto mt35 = new MotorGroupBox(MotorGroupBox::Motor_35);
    connect(mt35, &MotorGroupBox::sig_Add, this, &progConfig::listAdd);
    ui->configLayout->addWidget(mt35);

    auto mt57 = new MotorGroupBox(MotorGroupBox::Motor_57);
    connect(mt57, &MotorGroupBox::sig_Add, this, &progConfig::listAdd);
    ui->configLayout->addWidget(mt57);

    auto mt86 = new MotorGroupBox(MotorGroupBox::Motor_86);
    connect(mt86, &MotorGroupBox::sig_Add, this, &progConfig::listAdd);
    ui->configLayout->addWidget(mt86);

    auto mtRobot = new MotorGroupBox(MotorGroupBox::Motor_robot);
    connect(mtRobot, &MotorGroupBox::sig_Add, this, &progConfig::listAdd);
    ui->configLayout->addWidget(mtRobot);*/

    auto hjgp = new HeatGroupBox;
    connect(hjgp, &HeatGroupBox::sig_Add, this, &progConfig::listAdd);
    ui->configLayout->addWidget(hjgp);
    
    auto montain = new HeatGroupBox(HeatGroupBox::Dev_montain);
    connect(montain, &HeatGroupBox::sig_Add, this, &progConfig::listAdd);
    ui->configLayout->addWidget(montain);

    QFlowPanel *flowpanel = new QFlowPanel;
    connect(flowpanel,&QFlowPanel::flowPanelAdd,this, &progConfig::listAdd);
    ui->configLayout->addWidget(flowpanel);

    QSlopeTemp *slopetemp = new QSlopeTemp;
    connect(slopetemp,&QSlopeTemp::slopePanelAdd,this, &progConfig::listAdd);
    ui->configLayout->addWidget(slopetemp);

    /*QPumpCtrl *pumpCtrl = new QPumpCtrl;
    connect(pumpCtrl,&QPumpCtrl::pumpPanelAdd,this, &progConfig::listAdd);
    ui->configLayout->addWidget(pumpCtrl);*/

    QValveCtrl *valveCtrl = new QValveCtrl;
    connect(valveCtrl,&QValveCtrl::valvePanelAdd,this, &progConfig::listAdd);
    ui->configLayout->addWidget(valveCtrl);

    /*QCollectorCtrl *collCtrl = new QCollectorCtrl;
    connect(collCtrl, &QCollectorCtrl::collPanelAdd,this, &progConfig::listAdd);
    ui->configLayout->addWidget(collCtrl);
    collCtrl->setVisible(false);*/

    QSFunction *sfuntion = new QSFunction;
    connect(sfuntion, &QSFunction::functionPanelAdd,this, &progConfig::listAdd);
    ui->configLayout->addWidget(sfuntion);

    ui->btn_open->setIcon(QIcon(":/programConfig/image/open.png"));
    ui->btn_open->setIconSize(QSize(30,30));

    ui->btn_save->setIcon(QIcon(":/programConfig/image/save.png"));
    ui->btn_save->setIconSize(QSize(30,30));

    ui->btn_clear->setIcon(QIcon(":/programConfig/image/clear.png"));
    ui->btn_clear->setIconSize(QSize(30,30));

    ui->btn_append->setIcon(QIcon(":/programConfig/image/add.png"));
    ui->btn_append->setIconSize(QSize(30,30));

    ui->move_up->setIcon(QIcon(":/programConfig/image/up.png"));
    ui->move_up->setIconSize(QSize(30,30));

    ui->move_down->setIcon(QIcon(":/programConfig/image/down.png"));
    ui->move_down->setIconSize(QSize(30,30));

    ui->obj_del->setIcon(QIcon(":/programConfig/image/del.png"));
    ui->obj_del->setIconSize(QSize(30,30));

    QScroller::grabGesture(ui->scrollArea->viewport(),QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->scrollArea_2->viewport(),QScroller::LeftMouseButtonGesture);
}

progConfig::~progConfig()
{
    delete ui;
}

void progConfig::listAdd(const QString &str)
{
    ui->listWidget->addItem(str);
}

void progConfig::on_btn_open_clicked()
{
#ifndef Q_OS_WIN

    fileOpen *file_open = new fileOpen;
    file_open->listWidget = ui->listWidget;
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    int x = (screen.width() - file_open->width()) / 2;
    int y = (screen.height() - file_open->height()) / 2;
    file_open->move(x,y);

    file_open->raise();
    file_open->show();

#else

    QString path = QCoreApplication::applicationDirPath()+"/config";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    QString str = QFileDialog::getOpenFileName(this,"open",path,"(*.txt)");  // 打开文件
    if(str == "")
    {
       return;    //  “取消” 退出
    }
    QFile readFile(str);
    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;

    ui->listWidget->clear();  //  清空当前列表

    QTextStream stream(&readFile);    //  读取文件
    QString line;
    while (!stream.atEnd())
    {
        line = stream.readLine();   //  逐行读取
        if(line.isEmpty())
        {
           readFile.close();
           return ;
        }
        ui->listWidget->addItem(line);
    }
    readFile.close();

#endif
}

void progConfig::on_btn_save_clicked()
{
#ifndef Q_OS_WIN

    fileSave *file_save = new fileSave;
    file_save->listWidget = ui->listWidget;
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    int x = (screen.width() - file_save->width()) / 2;
    int y = (screen.height() - file_save->height()) / 2;
    file_save->move(x,y);

    file_save->raise();
    file_save->show();

#else

    QString path = QCoreApplication::applicationDirPath()+"/config";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    QString str = QFileDialog::getSaveFileName(this,"save",path,"(*.txt)");
    if(!str.isEmpty())
    {
        int cnt = ui->listWidget->count();
        if(cnt == 0)
        {
             QMessageBox::information(this,"提示","请添加项目！");
             return ;
        }
        QFile file(str);
        file.open( QIODevice::WriteOnly  | QIODevice::Text | QIODevice::Truncate);
        QTextStream out(&file);
        QString text;
        for(int i=0;i<cnt;i++)
        {
           text = ui->listWidget->item(i)->text() + "\n";
           out<<tr(text.toStdString().c_str());
        }
        file.close();
    }

#endif
}

void progConfig::on_btn_clear_clicked()
{
    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"确定清空?","提示",MyMessageBox::Yes | MyMessageBox::No);
    switch (result)
    {
        case MyMessageBox::Yes:
             ui->listWidget->clear();
             break;
        case MyMessageBox::No:

             break;
        default:
             break;
    }
}

void progConfig::on_btn_append_clicked()
{
#ifndef Q_OS_WIN

    fileOpen *file_open = new fileOpen;
    file_open->appendFlag = true;
    file_open->setTitle("追加文件");
    file_open->listWidget = ui->listWidget;
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    int x = (screen.width() - file_open->width()) / 2;
    int y = (screen.height() - file_open->height()) / 2;
    file_open->move(x,y);

    file_open->raise();
    file_open->show();

#else

    QString path = QCoreApplication::applicationDirPath()+"/config";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    QString str = QFileDialog::getOpenFileName(this,"open",path,"(*.txt)");  // 打开文件
    if(str == "")
    {
       return;    //  “取消” 退出
    }
    QFile readFile(str);
    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;

    QTextStream stream(&readFile);    //  读取文件
    QString line;
    while (!stream.atEnd())
    {
        line = stream.readLine();   //  逐行读取
        if(line.isEmpty())
        {
           readFile.close();
           return ;
        }
        ui->listWidget->addItem(line);
    }
    readFile.close();

#endif
}

void progConfig::on_move_up_clicked()
{
    if(ui->listWidget->currentRow() > 0)
    {
        int row = ui->listWidget->currentRow() - 1;
        QListWidgetItem *item1 = ui->listWidget->takeItem(ui->listWidget->currentRow() - 1);
        QListWidgetItem *item2 = ui->listWidget->takeItem(ui->listWidget->currentRow());

        ui->listWidget->insertItem(row,item2);
        ui->listWidget->insertItem(row+1,item1);

        ui->listWidget->setCurrentRow(row);
    }
}

void progConfig::on_move_down_clicked()
{
    if(ui->listWidget->currentRow() >= 0)
    {
        int row = ui->listWidget->currentRow();
        QListWidgetItem *item1 = ui->listWidget->takeItem(ui->listWidget->currentRow() + 1);  // 删除下一行
        QListWidgetItem *item2 = ui->listWidget->takeItem(ui->listWidget->currentRow());  // 删除当前行

        ui->listWidget->insertItem(row,item1);
        ui->listWidget->insertItem(row + 1,item2);

        ui->listWidget->setCurrentRow(row + 1);
    }
}

void progConfig::on_obj_del_clicked()
{
    QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
    delete  item;
}
