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
#include "progItem/swpanel.h"
#include "progItem/qflowpanel.h"
#include "progItem/qtimedelay.h"
#include "progItem/qcirculation.h"
#include "progItem/qponittemp.h"
#include "progItem/qslopetemp.h"
#include "progItem/qtcdpanel.h"
#include "progItem/QSFunction.h"
#include "progItem/qfunlabel.h"
#include "progItem/qpumpctrl.h"
#include "progItem/qvalvectrl.h"
#include "progItem/qcollectorctrl.h"
#include "progItem/MotorGroupBox.h"
#include "progItem/HeatGroupBox.h"
#include "progItem/FeedLiquidGroupBox.h"
#include "progItem/FeederGroupBox.h"
#include "progItem/AirWayGroupBox.h"
#include "ProgmaMgr.h"

#include "ui_progconfig.h"
#pragma execution_character_set("utf-8")

progConfig::progConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::progConfig)
{
    ui->setupUi(this);

    QFunLabel *funName = new QFunLabel;
    ui->configLayout->addWidget(funName);

    auto fg = new FeederGroupBox();
    ui->configLayout->addWidget(fg);

    auto liquid = new FeedLiquidGroupBox();
	ui->configLayout->addWidget(liquid);

	auto air = new AirWayGroupBox();
	ui->configLayout->addWidget(air);

    SwPanel *sw = new SwPanel;
    ui->configLayout->addWidget(sw);

#ifdef _DEBUG
    auto mt35 = new MotorGroupBox(MotorGroupBox::Motor_35);
    ui->configLayout->addWidget(mt35);

    auto mt57 = new MotorGroupBox(MotorGroupBox::Motor_57);
    ui->configLayout->addWidget(mt57);

    auto mt86 = new MotorGroupBox(MotorGroupBox::Motor_86);
    ui->configLayout->addWidget(mt86);

    auto mtRobot = new MotorGroupBox(MotorGroupBox::Motor_robot);
    ui->configLayout->addWidget(mtRobot);

    QPumpCtrl *pumpCtrl = new QPumpCtrl;
    ui->configLayout->addWidget(pumpCtrl);
#endif

    auto hjgp = new HeatGroupBox;
    ui->configLayout->addWidget(hjgp);
    
    auto montain = new HeatGroupBox(HeatGroupBox::Dev_montain);
    ui->configLayout->addWidget(montain);

    QFlowPanel *flowpanel = new QFlowPanel;
    ui->configLayout->addWidget(flowpanel);

    QSlopeTemp *slopetemp = new QSlopeTemp;
    ui->configLayout->addWidget(slopetemp);

    QValveCtrl *valveCtrl = new QValveCtrl;
    ui->configLayout->addWidget(valveCtrl);

    /*QCollectorCtrl *collCtrl = new QCollectorCtrl;
    ui->configLayout->addWidget(collCtrl);
    collCtrl->setVisible(false);*/

    QSFunction *sfuntion = new QSFunction;
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
       dir.mkdir(path);

    QString str = QFileDialog::getOpenFileName(this, "open", path, "*.prg");  // 打开文件
    if (!str.isEmpty())
        ProgmaMgr::Instance().Load(str);
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
       dir.mkdir(path);

    QString str = QFileDialog::getSaveFileName(this, "save", path, "*.prg");
    if (!str.isEmpty())
        ProgmaMgr::Instance().Save(str);
#endif
}

void progConfig::on_btn_clear_clicked()
{
    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"确定清空?","提示",MyMessageBox::Yes | MyMessageBox::No);
    switch (result)
    {
        case MyMessageBox::Yes:
			ProgmaMgr::Instance().Clear();
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
       dir.mkdir(path);
  
	QString str = QFileDialog::getOpenFileName(this, "open", path, "*.prg");  // 打开文件
	if (!str.isEmpty())
		ProgmaMgr::Instance().Append(str);

#endif
}

void progConfig::on_move_up_clicked()
{
	auto idx = ui->listWidget->currentRow();
    if(idx > 0)
		ProgmaMgr::Instance().MoveUpAt(idx);
}

void progConfig::on_move_down_clicked()
{
	auto idx = ui->listWidget->currentRow();
	if (idx > 0)
		ProgmaMgr::Instance().MoveDownAt(idx);
}

void progConfig::on_obj_del_clicked()
{
    QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
    delete  item;
}
