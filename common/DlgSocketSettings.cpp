#include "DlgSocketSettings.h"
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ui_DlgSocketSettings.h"
#pragma execution_character_set("utf-8")

DlgSocketSettings::DlgSocketSettings(QWidget *parent) : QDialog(parent)
, m_ui(new Ui::DlgSocketSettings)
{
    m_ui->setupUi(this);
    initUi();
}

DlgSocketSettings::~DlgSocketSettings()
{
    delete m_ui;
}


QString DlgSocketSettings::GetHost() const
{
    return m_ui->edit_host->text();
}

uint16_t DlgSocketSettings::GetPort() const
{
    return m_ui->spin_port->value();
}

void DlgSocketSettings::Inital(const QString &host, int port)
{
    m_ui->edit_host->setText(host.isEmpty() ? "127.0.0.1" : host);
    m_ui->spin_port->setValue(port);
}

void DlgSocketSettings::initUi()
{
    connect(m_ui->btn_cancle, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_ui->btn_ok, &QPushButton::clicked, this, &DlgSocketSettings::accept);
}
