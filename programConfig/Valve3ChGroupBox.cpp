#include "Valve3ChGroupBox.h"

#include "Ui_Valve3ChGroupBox.h"
#pragma execution_character_set("utf-8")

FeedLiquidGroupBox::FeedLiquidGroupBox(QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::FeedLiquidGroupBox)
{
    m_ui->setupUi(this);
    m_ui->pushButton->setEnabled(false);
    connect(m_ui->pushButton, &QPushButton::clicked, this, [=] {
        emit sig_Add(_getOutString());
    });
    connect(m_ui->spin, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &FeedLiquidGroupBox::onValueChanged);
    connect(m_ui->spin_air, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FeedLiquidGroupBox::onValueChanged);
 //   connect(m_ui->spin_feed, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FeedLiquidGroupBox::onValueChanged);
}

FeedLiquidGroupBox::~FeedLiquidGroupBox()
{
    delete m_ui;
}

QString FeedLiquidGroupBox::_getOutString() const
{
    return tr("取液 通道 %1 流速(ml/s) %2 排空时间(s) %3 配液时间(s) %4").arg(m_ui->cmb_out->currentIndex()+1)
        .arg(m_ui->spin->value()).arg(m_ui->spin_air->value())/*.arg(m_ui->spin_feed->value())*/;
}

void FeedLiquidGroupBox::onValueChanged()
{
    bool b = m_ui->spin->value() > 0 && m_ui->spin_air->value() > 0/* && m_ui->spin_feed->value() > 0*/;
    m_ui->pushButton->setEnabled(b);
}
