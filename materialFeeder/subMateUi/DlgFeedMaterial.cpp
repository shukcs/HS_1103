#include "DlgFeedMaterial.h"
#include "../FeederMgr.h"

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

bool DlgFeedMaterial::GetFeedParam(QMap<QString, float>* mates)const
{
    if (m_ui->cmb->currentIndex() == 1)
    {
        m_ui->widget->GetFeedMaterials(mates);
        return true;
    }
    return false;
}

int DlgFeedMaterial::GetTubeNumb()const
{
    auto tub = FeederMgr::Instance().ValidTube(m_ui->widget->GetTubeNumber());
    return tub ? tub->getNumber() : -1;
}

void DlgFeedMaterial::Init(const FeederParam* pr, BottleStruct *bt)
{
    QSet<BottleStat> sChs = { B_CanUse, B_None };
    auto st = bt ? bt->getFlag() : B_Using;
    m_ui->cmb->setEnabled(sChs.contains(st));
    m_ui->w_s->setVisible(st!=B_Used);
    if (B_Used == st)
        setWindowTitle(tr("料瓶%1清洗后归位").arg(bt->m_numb + 1));
    else
        setWindowTitle(tr("料瓶%1投料").arg(bt->m_numb + 1));
    m_ui->widget->SetFeedMaterials(pr);
    setBottleAvlible(bt && bt->getFlag()==T_WaitPrepare);
}

void DlgFeedMaterial::setBottleAvlible(bool b)
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
    connect(m_ui->cmb, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, [=](const QString &str) {
        static QList<QString> sSet = { tr("料瓶可用"),  tr("缺失") };
        auto idx = sSet.indexOf(str);
        if (idx > 0)
            emit bottleAvlibleChanged(idx== tr("料瓶可用"));
        setBottleAvlible(idx = 0);
    });
}