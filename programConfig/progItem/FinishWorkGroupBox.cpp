#include "FinishWorkGroupBox.h"
#include "DevContrlMgr/CtrlAction.h"
#include "materialFeeder/FeederActionItem.h"
#include "ui_FinishWorkGroupBox.h"
#pragma execution_character_set("utf-8")

FinishWorkGroupBox::FinishWorkGroupBox(QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::FinishWorkGroupBox)
{
    m_ui->setupUi(this);
    connect(m_ui->pushButton, &QPushButton::clicked, this, [=] {
        auto ch = m_ui->cmb_ch->currentIndex() == 0 ? 0 : 1;
        auto actH = new HeatAction(0xffff, ch);
        actH->AddToEdit();
        auto actK = new KeepAction(0xffff, ch);
        actK->AddToEdit();
        auto actSt = new StoveHeatAction(ch);
        actSt->AddToEdit();
        auto actAirEnd = new AirEndAction(ch);
        actAirEnd->AddToEdit();
        auto actLiEnd = new LiquidEndAction(ch);
        actLiEnd->AddToEdit();
        auto delay = new DelayAction(m_ui->spin_delay->value() / 60.0);
        delay->AddToEdit();
        auto back = new TubeBackItem(ch, m_ui->cmb_back->currentIndex());
        back->AddToEdit();
    });
}

FinishWorkGroupBox::~FinishWorkGroupBox()
{
    delete m_ui;
}
