#include "AirWayGroupBox.h"
#include "strDecoder/strdecoder.h"

#include "Ui_AirWayGroupBox.h"
#pragma execution_character_set("utf-8")

AirWayGroupBox::AirWayGroupBox(QWidget *parent /*= nullptr*/):QGroupBox(parent)
, m_ui(new Ui::AirWayGroupBox)
{
    m_ui->setupUi(this);
	initUi();
}

AirWayGroupBox::~AirWayGroupBox()
{
    delete m_ui;
}

void AirWayGroupBox::initUi()
{
	connect(m_ui->btn_clr, &QPushButton::clicked, this, &AirWayGroupBox::onClear);
	connect(m_ui->btn_in, &QPushButton::clicked, this, &AirWayGroupBox::onAirIn);
}

void AirWayGroupBox::onClear()
{
	QString str = tr("%1 吹扫气压 %2 MPa 吹扫时间 %3 s").arg(m_ui->lb_clr->text()).arg(m_ui->spin_prsClr->text()).arg(m_ui->spinBox->text());
	emit sig_Add(str);
}

void AirWayGroupBox::onAirIn()
{
	QString str = tr("%1 进气气压 %2 MPa 流速 %3 ml/s 背压阀 %4").arg(m_ui->lb_in->text()).arg(m_ui->spin_prsIn->text())
		.arg(m_ui->spin_speed->text()).arg(m_ui->spin_speed->text());
	emit sig_Add(str);
}
