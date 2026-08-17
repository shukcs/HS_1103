#include "HeatGroupBox.h"

#include "Ui_HeatGroupBox.h"
#pragma execution_character_set("utf-8")

HeatGroupBox::HeatGroupBox(Dev_type tp, QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::HeatGroupBox)
{
    m_ui->setupUi(this);
    if (Dev_montain == tp)
        setTitle(tr("保温箱"));

    connect(m_ui->pushButton, &QPushButton::clicked, this, &HeatGroupBox::onAdd);
}

HeatGroupBox::~HeatGroupBox()
{
    delete m_ui;
}

QString HeatGroupBox::_getChStr()const
{
    switch (m_ui->cmb_ch->currentIndex())
    {
    case 0:
        return  tr(" 通道 1");
    case 1:
        return  tr(" 通道 2");
    case 2:
        return  tr(" 通道 1 2");
    default:
        break;
    }
    return QString();
}

QString HeatGroupBox::_getTmpString() const
{
    if (m_ui->spinBox->value() == -1)
        return tr(" 停止加热");

    return tr(" 温度 %1").arg(m_ui->spinBox->value());
}

void HeatGroupBox::onAdd()
{
    emit sig_Add(title() + _getChStr() + _getTmpString());
}