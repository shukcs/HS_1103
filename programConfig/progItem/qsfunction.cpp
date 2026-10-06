#include "QSFunction.h"
#include "DevContrlMgr/CtrlAction.h"
#include "DevContrlMgr/DevContrlMgr.h"
#include "ui_QSFunction.h"
#pragma execution_character_set("utf-8")

QSFunction::QSFunction(QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::QSFunction)
{
	m_ui->setupUi(this);
	initUi();
}

QSFunction::~QSFunction()
{
	delete m_ui;
}

void QSFunction::initUi()
{
	connect(m_ui->btn_rec, &QPushButton::clicked, this, &QSFunction::onRec);
	connect(m_ui->btn_delay, &QPushButton::clicked, this, &QSFunction::onDelay);
	connect(m_ui->btn_cyc, &QPushButton::clicked, this, &QSFunction::onCycle);
}

void QSFunction::onRec()
{
    if (auto act = new RecordAction(m_ui->cmb_rec->currentIndex()==0))
        act->AddToEdit();
}

void QSFunction::onDelay()
{
    if (auto act = new DelayAction(m_ui->spin_time->value()))
        act->AddToEdit();
}

void QSFunction::onCycle()
{
    if (auto act = new CycleAction(m_ui->cmb_cyc->currentIndex() == 0 ? m_ui->spin_cyc->value() : 0))
        act->AddToEdit();
}
