#include "FeedLiquidGroupBox.h"
#include "strDecoder/CtrlAction.h"
#include "Ui_FeedLiquidGroupBox.h"
#pragma execution_character_set("utf-8")

FeedLiquidGroupBox::FeedLiquidGroupBox(QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::FeedLiquidGroupBox)
{
    m_ui->setupUi(this);
    connect(m_ui->pushButton, &QPushButton::clicked, this, [=] {
        if (m_ui->cmb_feed->currentIndex() == 0)
        {
            if (auto act = new LiquidInAction(m_ui->cmb_out->currentIndex(), m_ui->spin->value(), m_ui->spin_air->value()))
                act->AddToEdit();
        }
        else if (auto act = new LiquidEndAction(m_ui->cmb_out->currentIndex()))
        {
            act->AddToEdit();
        }
    });
    connect(m_ui->spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &FeedLiquidGroupBox::onValueChanged);
}

FeedLiquidGroupBox::~FeedLiquidGroupBox()
{
    delete m_ui;
}

void FeedLiquidGroupBox::onValueChanged(double f)
{
	int tm = 6.5 / f * 60;
	if (tm < 1)
		tm = 1;
	m_ui->spin_air->setValue(tm);
}
