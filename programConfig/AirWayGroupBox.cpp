#include "AirWayGroupBox.h"
#include "strDecoder/strdecoder.h"

#include "Ui_AirWayGroupBox.h"
#pragma execution_character_set("utf-8")

AirWayGroupBox::AirWayGroupBox(QWidget *parent /*= nullptr*/):QGroupBox(parent)
, m_ui(new Ui::AirWayGroupBox)
{
    m_ui->setupUi(this);
}

AirWayGroupBox::~AirWayGroupBox()
{
    delete m_ui;
}

void AirWayGroupBox::initUi()
{

}

void AirWayGroupBox::onClear()
{

}

void AirWayGroupBox::onAirIn()
{

}
