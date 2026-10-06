#include "desktop.h"
#include "materialFeeder/MaterialStore.h"
#include "materialFeeder/FeederMgr.h"
#include "manualOp/manualop.h"
#include "deviceOp/deviceop.h"
#include "customGraph/customgraph.h"
#include "programConfig/progconfig.h"
#include "DevContrlMgr/DevContrlMgr.h"
#include "projectMode/projectmode.h"
#include "collectorOp/collectorop.h"
#include "Diagram/diagram.h"
#include "HsApplication.h"
#include "materialFeeder/FeederMgr.h"

#include "ui_desktop.h"
#pragma execution_character_set("utf-8")

deskTop::deskTop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::deskTop)
{
    ui->setupUi(this);
    setWindowTitle("双通道高压固定床反应器ADFD-2000");
    setWindowFlag(Qt::FramelessWindowHint);

    decoder = new DevContrlMgr(this, ui->state->port.name);   ///字符串转命令

    ui->configBtn->setIcon(QIcon(":/image/set.png"));
    ui->configBtn->setIconSize(QSize(25, 25));

    manualOperation = new manualOp;   //  主页界面
    ui->mainUI->addWidget(manualOperation);

    deviceOperation = new deviceOp;     //  手动界面
    ui->mainUI->addWidget(deviceOperation);

    m_graph = hsApp->getGustomGraph();     // 曲线界面
    ui->mainUI->addWidget(m_graph);

    progconfig = new progConfig;
    ui->mainUI->addWidget(progconfig);   // 程序配置界面

    projectmode = new projectMode;
    ui->mainUI->addWidget(projectmode);   // 隐藏界面

    m_mateSt = new MaterialStore;
    ui->mainUI->addWidget(m_mateSt);   // 隐藏界面

    connect(manualOperation, &manualOp::cmdTorun, decoder, &DevContrlMgr::strTocmd);
    connect(deviceOperation, &deviceOp::cmdTorun, decoder, &DevContrlMgr::strTocmd);
    connect(deviceOperation, SIGNAL(deviceOp::cmdTorun(const QString&)), manualOperation, SLOT(strToState(const QString&)));
    connect(projectmode, &projectMode::cmdTorun, decoder, &DevContrlMgr::strTocmd);

    collectorOperation = new collectorOp;     //  收集器界面
    ui->mainUI->addWidget(collectorOperation);
    connect(collectorOperation, &collectorOp::cmdTorun, decoder, &DevContrlMgr::strTocmd);

    //connect(decoder,SIGNAL(setConnectionState(bool)),deviceOperation,SLOT(update_user_set(bool)));
    //connect(decoder, SIGNAL(setConnectionState(bool)), ui->state, SLOT(setConnectionState(bool)));
    //connect(decoder->getThread(), &portThread::dateReceived, this, &deskTop::updateInfo);
    //connect(decoder->getThread(), &portThread::heatAndKeepChanged, manualOperation->getDiagram(), &diagram::updateHeatAndKeep);
    //connect(decoder->getThread(), &portThread::triEleValveStat, manualOperation->getDiagram(), &diagram::updateTriEleValveStat);
    //connect(decoder,SIGNAL(startRecord(bool)),graph,SLOT(startRecord(bool)));
    //connect(decoder,SIGNAL(autoSavedata()),graph,SLOT(autoSavedata()));
    //connect(ui->state, &stateBar::portChanged, decoder, &DevContrlMgr::savePortName);
    //connect(ui->state,SIGNAL(sampleTimeChanged(int)),graph,SLOT(setTimer(int)));
    //connect(ui->state,SIGNAL(sampleTimeChanged(int)),decoder,SLOT(setTimer(int)));
    //connect(ui->tool,SIGNAL(logoBtn_clicked()), deviceOperation,SLOT(tempAdjustEnable()));

    connect(ui->autoRunStep, &ProgramList::readyTorun, decoder, &DevContrlMgr::strTocmd);
    connect(ui->autoRunStep, &ProgramList::readyTorun_toColl, collectorOperation, &collectorOp::updateCmd);
    connect(ui->autoRunStep, &ProgramList::sendRunTime, this, &deskTop::updateRunTime);
    connect(ui->autoRunStep,SIGNAL(readyTorun(const QString&)),manualOperation,SLOT(strToState(const QString&)));

    connect(ui->autoRunStep,SIGNAL(autoRun(bool)),manualOperation,SLOT(autoRun(bool)));
    connect(ui->autoRunStep,SIGNAL(autoRun(bool)),deviceOperation,SLOT(autoRun(bool)));
}

deskTop::~deskTop()
{
    delete ui;
}

DevContrlMgr * deskTop::getDevContrlMgr() const
{
	return decoder;
}

deviceOp * deskTop::getDeviceOp() const
{
    return deviceOperation;
}

void deskTop::Scene1Show()
{
    ui->mainUI->setCurrentWidget(manualOperation);
}

void deskTop::Scene2Show()
{
    ui->mainUI->setCurrentWidget(deviceOperation);
}

void deskTop::Scene3Show()
{
    ui->mainUI->setCurrentWidget(m_graph);
}

void deskTop::Scene4Show()
{
    ui->mainUI->setCurrentWidget(progconfig);
}

void deskTop::Scene5Show()
{
    ui->mainUI->setCurrentWidget(projectmode);
}

void deskTop::Scene6Show()
{
    ui->mainUI->setCurrentWidget(collectorOperation);
}

void deskTop::StoreShow()
{
    ui->mainUI->setCurrentWidget(m_mateSt);
}

void deskTop::updateRunTime(int time)
{
   ui->runTime->setText(QString::number(time/60) + ":"+QString::number(time%60));
}

void deskTop::on_configBtn_clicked()
{
    Scene4Show();
}
