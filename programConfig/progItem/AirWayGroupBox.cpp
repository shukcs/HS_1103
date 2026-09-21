#include "AirWayGroupBox.h"
#include "strDecoder/strdecoder.h"
#include "strDecoder/CtrlAction.h"

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
    connect(m_ui->spin_prsIn, &QAbstractSpinBox::editingFinished, this, [=] {
        auto v = m_ui->spin_prsIn->value() - .1;
        if (v < 0)
            v = 0;
        m_ui->spin_valve->setValue(v);
    });
}

void AirWayGroupBox::onClear()
{
    if (auto act = new AirClrAction(m_ui->cmb_ch->currentIndex(), m_ui->spin_prsClr->value(), m_ui->spinBox->value()))
        act->AddToEdit();
}

void AirWayGroupBox::onAirIn()
{
    if (auto act = new AirInAction(m_ui->cmb_ch->currentIndex(), m_ui->spin_prsIn->value(), m_ui->spin_speed->value(), m_ui->spin_valve->value()))
        act->AddToEdit();
}
