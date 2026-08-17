#include "SpinCust.h"

#include "ui_SpinCust.h"
#pragma execution_character_set("utf-8")

SpinCust::SpinCust(QWidget *parent) : QWidget(parent)
, m_ui(new Ui::SpinCust)
{
    m_ui->setupUi(this);
    connect(m_ui->btn_add, &QPushButton::clicked, this, [=] {emit btnClick(0); });
    connect(m_ui->btn_del, &QPushButton::clicked, this, [=] {emit btnClick(1); });
}

SpinCust::~SpinCust()
{
    delete m_ui;
}

void SpinCust::SetWeight(double w)
{
    m_ui->spinBox->setValue(w);
}

void SpinCust::SetMaxWeight(double w)
{
    m_ui->spinBox->setMaximum(w);
}

double SpinCust::GetWeight()const
{
    return m_ui->spinBox->value();
}

void SpinCust::SetBtnVisible(bool b, int idx)
{
    auto btn = idx == 0 ? m_ui->btn_add : m_ui->btn_del;
    btn->setVisible(b);
}