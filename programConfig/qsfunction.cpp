#include "QSFunction.h"

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
    emit functionPanelAdd(tr("曲线记录 %1").arg(m_ui->cmb_rec->currentText()));
}

void QSFunction::onDelay()
{
    emit functionPanelAdd(tr("延时 %1 min").arg(m_ui->spin_time->text()));
}

void QSFunction::onCycle()
{
	if (m_ui->cmb_cyc->currentIndex() == 0)
		emit functionPanelAdd(tr("开始循环 %1 次").arg(m_ui->spin_cyc->value()));
	else
		emit functionPanelAdd("结束循环");
}
