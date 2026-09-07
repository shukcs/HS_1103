#include "desktop.h"
#include "materialFeeder/MaterialStore.h"
#include "materialFeeder/FeederMgr.h"
#include "manualOp/manualop.h"
#include "deviceOp/deviceop.h"
#include "customGraph/customgraph.h"
#include "programConfig/progconfig.h"
#include "strDecoder/strdecoder.h"
#include "projectMode/projectmode.h"
#include "collectorOp/collectorop.h"
#include "Diagram/diagram.h"

#include "ui_desktop.h"
#pragma execution_character_set("utf-8")

deskTop::deskTop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::deskTop)
{
    ui->setupUi(this);
    setWindowTitle("双通道高压固定床反应器ADFD-2000");
    setWindowFlag(Qt::FramelessWindowHint);

    decoder = new strDecoder(this, ui->state->port.name);   //  字符串转命令

    ui->configBtn->setIcon(QIcon(":/image/set.png"));
    ui->configBtn->setIconSize(QSize(25, 25));

    manualOperation = new manualOp;   //  主页界面
    ui->mainUI->addWidget(manualOperation);

    deviceOperation = new deviceOp;     //  手动界面
    ui->mainUI->addWidget(deviceOperation);

    graph = new customGraph;     //  曲线界面
    ui->mainUI->addWidget(graph);

    progconfig = new progConfig;
    ui->mainUI->addWidget(progconfig);   // 程序配置界面

    projectmode = new projectMode;
    ui->mainUI->addWidget(projectmode);   // 隐藏界面

    m_mateSt = new MaterialStore;
    ui->mainUI->addWidget(m_mateSt);   // 隐藏界面

    connect(manualOperation, &manualOp::cmdTorun, decoder, &strDecoder::strTocmd);
    connect(deviceOperation, &deviceOp::cmdTorun, decoder, &strDecoder::strTocmd);
    connect(deviceOperation, SIGNAL(deviceOp::cmdTorun(const QString&)), manualOperation, SLOT(strToState(const QString&)));
    connect(projectmode, &projectMode::cmdTorun, decoder, &strDecoder::strTocmd);

    collectorOperation = new collectorOp;     //  收集器界面
    ui->mainUI->addWidget(collectorOperation);
    connect(collectorOperation, &collectorOp::cmdTorun, decoder, &strDecoder::strTocmd);

    //connect(decoder,SIGNAL(setConnectionState(bool)),deviceOperation,SLOT(update_user_set(bool)));
    connect(decoder, &strDecoder::setConnectionState, projectmode, &projectMode::update_user_set);
    connect(decoder, SIGNAL(setConnectionState(bool)), ui->state, SLOT(setConnectionState(bool)));
    connect(decoder->getThread(), &portThread::ReceiceDone, this, &deskTop::updateInfo);
    connect(decoder->getThread(), &portThread::heatAndKeepChanged, manualOperation->getDiagram(), &diagram::updateHeatAndKeep);
    connect(decoder->getThread(), &portThread::triEleValveStat, manualOperation->getDiagram(), &diagram::updateTriEleValveStat);
    connect(decoder,SIGNAL(startRecord(bool)),graph,SLOT(startRecord(bool)));
    connect(decoder,SIGNAL(autoSavedata()),graph,SLOT(autoSavedata()));
    connect(ui->state, &stateBar::portChanged, decoder, &strDecoder::savePortName);
    connect(ui->state,SIGNAL(sampleTimeChanged(int)),graph,SLOT(setTimer(int)));
    connect(ui->state,SIGNAL(sampleTimeChanged(int)),decoder,SLOT(setTimer(int)));

    connect(ui->state,SIGNAL(autoBtn_clicked()),this,SLOT(Scene1Show()));
    connect(ui->state,SIGNAL(manualBtn_clicked()),this,SLOT(Scene2Show()));
    connect(ui->state,SIGNAL(collectorBtn_clicked()),this,SLOT(Scene6Show()));
    connect(ui->state, SIGNAL(graphBtn_clicked()), this, SLOT(Scene3Show()));
    connect(ui->state, &stateBar::storeBtn_clicked, this, &deskTop::StoreShow);
    connect(ui->tool,SIGNAL(logoBtn_clicked()),deviceOperation,SLOT(tempAdjustEnable()));

    connect(ui->autoRunStep, &ProgramList::readyTorun, decoder, &strDecoder::strTocmd);
    connect(decoder, &strDecoder::jobChaned, ui->autoRunStep, &ProgramList::OnStoveTubeChanged);
    connect(ui->autoRunStep, &ProgramList::readyTorun_toColl, collectorOperation, &collectorOp::updateCmd);
    connect(ui->autoRunStep, &ProgramList::sendRunTime, this, &deskTop::updateRunTime);
    connect(ui->autoRunStep,SIGNAL(readyTorun(const QString&)),manualOperation,SLOT(strToState(const QString&)));

    connect(ui->autoRunStep,SIGNAL(autoRun(bool)),manualOperation,SLOT(autoRun(bool)));
    connect(ui->autoRunStep,SIGNAL(autoRun(bool)),deviceOperation,SLOT(autoRun(bool)));

    connect(ui->tool,SIGNAL(logoBtn_clicked()),this,SLOT(Scene5Show()));
}

deskTop::~deskTop()
{
    delete ui;
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
    ui->mainUI->setCurrentWidget(graph);
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

void deskTop::updateInfo(ReceiveData *data)
{
    manualOperation->updateInfo(data);   // 刷新相应界面内的数据
    deviceOperation->updateInfo(data);
    collectorOperation->updateInfo(data);
    graph->updateInfo(data);
    projectmode->updateInfo(data);
}

void deskTop::updateRunTime(int time)
{
   ui->runTime->setText(QString::number(time/60) + ":"+QString::number(time%60));
}

void deskTop::on_configBtn_clicked()
{
    Scene4Show();
}
