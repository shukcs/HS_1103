#include "FeedLiquidGroupBox.h"

#include "Ui_FeedLiquidGroupBox.h"
#pragma execution_character_set("utf-8")

FeedLiquidGroupBox::FeedLiquidGroupBox(QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::FeedLiquidGroupBox)
{
    m_ui->setupUi(this);
    connect(m_ui->pushButton, &QPushButton::clicked, this, [=] {
        emit sig_Add(_getOutString());
    });
    connect(m_ui->spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &FeedLiquidGroupBox::onValueChanged);
}

FeedLiquidGroupBox::~FeedLiquidGroupBox()
{
    delete m_ui;
}

QString FeedLiquidGroupBox::_getOutString() const
{
	if (m_ui->cmb_feed->currentIndex() == 0)
		return tr("取液 通道 %1 流速(ml/s) %2 排空时间(s) %3 %4").arg(m_ui->cmb_out->currentIndex()+1)
        .arg(m_ui->spin->value()).arg(m_ui->spin_air->value()).arg(m_ui->cmb_feed->currentText());

	return tr("取液 %1").arg(m_ui->cmb_feed->currentText());
}

void FeedLiquidGroupBox::onValueChanged(double f)
{
	int tm = 6.5 / f * 60;
	if (tm < 1)
		tm = 1;
	m_ui->spin_air->setValue(tm);
}
