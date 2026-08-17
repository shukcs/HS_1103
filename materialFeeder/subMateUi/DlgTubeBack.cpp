#include "DlgTubeBack.h"
#include "../FeederDecoder.h"

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
        m_ui->lb_numb->setText(QString::number(tb->numb+1));
}

void DlgTubeBack::initUi()
{
    connect(m_ui->btn_ok, &QPushButton::clicked, this, [=] {

        auto idx = m_ui->lb_numb->text().toInt();
        if (idx > 0)
            FeederDecoder::Instance().StoveTubeBack(idx - 1);
        accept();
    });
}