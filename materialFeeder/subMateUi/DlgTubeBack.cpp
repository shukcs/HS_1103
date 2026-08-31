#include "DlgTubeBack.h"
#include "../FeederMgr.h"

#include "ui_DlgTubeBack.h"

DlgTubeBack::DlgTubeBack(QWidget *parent) : QDialog(parent)
, m_ui(new Ui::DlgTubeBack)
{
    m_ui->setupUi(this);
    initUi();
}

DlgTubeBack::~DlgTubeBack()
{
    delete m_ui;
}

void DlgTubeBack::Init(const TubeStruct* tb)
{
    if (tb)
    {
        m_ui->lb_numb->setText(QString::number(tb->getNumber() + 1));
        m_ui->lb_tipback->setVisible(tb->getFlag() == T_Fixed);
        m_ui->lb_tipclear->setVisible(tb->getFlag() == T_Recyced);
    }
}

void DlgTubeBack::initUi()
{
    connect(m_ui->btn_ok, &QPushButton::clicked, this, &QDialog::accept);
}