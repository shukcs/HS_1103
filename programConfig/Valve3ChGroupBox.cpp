#include "Valve3ChGroupBox.h"

#include "Ui_Valve3ChGroupBox.h"
#pragma execution_character_set("utf-8")

Valve3ChGroupBox::Valve3ChGroupBox(QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::Valve3ChGroupBox)
{
    m_ui->setupUi(this);
    connect(m_ui->pushButton, &QPushButton::clicked, this, [=] {
        emit sig_Add(title() + _getChStr() + _getOutString());
    });
}

Valve3ChGroupBox::~Valve3ChGroupBox()
{
    delete m_ui;
}

QString Valve3ChGroupBox::_getChStr()const
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

QString Valve3ChGroupBox::_getOutString() const
{
    return tr(" 通向 %1").arg(m_ui->cmb_out->currentIndex());
}