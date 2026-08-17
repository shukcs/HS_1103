#include "DlgSerialSettings.h"
#include <QSerialPort>
#include <QSerialPortInfo>

#include "ui_DlgSerialSettings.h"
#pragma execution_character_set("utf-8")

QMap<int, QSerialPort::StopBits> sSTopIndexs = { {0, QSerialPort::OneStop }, {1, QSerialPort::OneAndHalfStop },{ 2, QSerialPort::TwoStop } };
QMap<int, QSerialPort::Parity> sParity = { { 0, QSerialPort::NoParity },{ 1, QSerialPort::EvenParity },{ 2, QSerialPort::OddParity } };

DlgSerialSettings::DlgSerialSettings(QWidget *parent) : QDialog(parent)
, m_ui(new Ui::DlgSerialSettings)
{
    m_ui->setupUi(this);
    initUi();
}

DlgSerialSettings::~DlgSerialSettings()
{
    delete m_ui;
}


void DlgSerialSettings::Inital(QSerialPort *port, bool bBase)
{
    m_port = port;
    if (port)
    {
        m_ui->cmb_name->setCurrentText(port->portName());
        m_ui->cmb_buat->setCurrentText(QString::number(port->baudRate()));
        m_ui->cmb_bit->setCurrentText(QString::number(port->dataBits()));
        m_ui->cmb_stop->setCurrentIndex(sSTopIndexs.key(port->stopBits(), 0));
        m_ui->cmb_parity->setCurrentIndex(sParity.key(port->parity(), 0));
    }
    if (bBase)
        m_ui->widget->setVisible(false);
}

void DlgSerialSettings::initUi()
{
    for (auto& itr : QSerialPortInfo::availablePorts())
    {
        m_ui->cmb_name->addItem(itr.portName());
    }
    connect(m_ui->btn_cancle, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_ui->btn_ok, &QPushButton::clicked, this, &DlgSerialSettings::connectPort);
    for (auto itr : findChildren<QComboBox*>())
    {
        connect(itr, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=] {m_bChanged = true; });
    }
}

void DlgSerialSettings::connectPort()
{
    if (!m_port) return;
    if (!m_bChanged && m_port->isOpen()) return;

    if (m_port->isOpen())
        m_port->close();

    m_port->setPortName(m_ui->cmb_name->currentText());
    m_port->setBaudRate(QSerialPort::BaudRate(m_ui->cmb_buat->currentText().toInt()));
    m_port->setStopBits(sSTopIndexs.value(m_ui->cmb_stop->currentIndex(), QSerialPort::OneStop));
    m_port->setParity(sParity.value(m_ui->cmb_parity->currentIndex(), QSerialPort::NoParity));
    m_port->setDataBits((QSerialPort::DataBits)m_ui->cmb_bit->currentText().toInt());
    m_port->open(QSerialPort::ReadWrite);

    accept();
}
