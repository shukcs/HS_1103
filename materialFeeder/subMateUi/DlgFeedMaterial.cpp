#include "DlgFeedMaterial.h"
#include "../FeederDecoder.h"

#include "ui_DlgFeedMaterial.h"
#pragma execution_character_set("utf-8")

DlgFeedMaterial::DlgFeedMaterial(QWidget *parent) : QDialog(parent)
, m_ui(new Ui::DlgFeedMaterial)
{
    m_ui->setupUi(this);
    initUi();
}

DlgFeedMaterial::~DlgFeedMaterial()
{
    delete m_ui;
}

bool DlgFeedMaterial::FeedBottle(QMap<QString, float>* mates)
{
    if (m_ui->cmb->currentIndex() == 1)
    {
        m_ui->widget->GetFeedMaterials(mates);
        return true;
    }
    return false;
}

int DlgFeedMaterial::GetTubeNumb()
{
    auto tub = FeederMgr::Instance().ValidTube(m_ui->widget->GetTubeNumber());
    return tub ? tub->getNumber() : -1;
}

void DlgFeedMaterial::Init(const FeederParam* bt)
{
    m_ui->widget->SetFeedMaterials(bt);
}

void DlgFeedMaterial::SetBottleAvlible(bool b)
{
    m_ui->cmb->setCurrentIndex(b ? 1 : 0);
    m_ui->widget->setEnabled(b);
}

int DlgFeedMaterial::GetChannel() const
{
    return m_ui->widget->GetChannel();
}

void DlgFeedMaterial::initUi()
{
    connect(m_ui->cmb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        emit bottleAvlibleChanged(idx==1);
        SetBottleAvlible(idx == 1);
    });
}