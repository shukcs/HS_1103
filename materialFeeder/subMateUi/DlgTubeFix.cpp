#include "DlgTubeFix.h"
#include "../FeederMgr.h"

#include "ui_DlgTubeFix.h"

DlgTubeFix::DlgTubeFix(QWidget *parent) : QDialog(parent)
, m_ui(new Ui::DlgTubeFix)
{
    m_ui->setupUi(this);
    initUi();
}

DlgTubeFix::~DlgTubeFix()
{
    delete m_ui;
}

void DlgTubeFix::Init(const TubeStruct* tb)
{
    if (tb)
        m_ui->lb_numb->setText(QString::number(tb->getNumber() + 1));

    m_ui->btn_ok->setVisible(tb != nullptr);
}

void DlgTubeFix::initUi()
{
    connect(m_ui->btn_cancle, &QPushButton::clicked, this, &QDialog::rejected);
    connect(m_ui->btn_ok, &QPushButton::clicked, this, [=] {
        FeederMgr::Instance().FixTube(m_ui->lb_numb->text().toInt() - 1, m_ui->cmb_ch->currentIndex());
        accepted();
    });
}