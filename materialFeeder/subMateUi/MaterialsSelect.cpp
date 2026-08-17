#include "MaterialsSelect.h"
#include <qEvent.h>
#include <QMap>
#include "../FeederDecoder.h"
#include "common/MaskWidget.h"

#include "ui_MaterialsSelect.h"
#pragma execution_character_set("utf-8")

MaterialsSelect::MaterialsSelect(QWidget *parent) : QWidget(parent)
, m_ui(new Ui::MaterialsSelect)
{
    m_ui->setupUi(this);
    m_ui->widget->setVisible(false);
    for (auto itr : FeederDecoder::Instance().ValidTubes())
    {
        m_ui->cmb_tube->addItem(tr("反应管%1").arg(itr->numb + 1));
    }
    initUi(m_ui->cmb_id, m_ui->w_id);

    connect(&FeederDecoder::Instance(), &FeederDecoder::matesCanFeedChanged, this, [=]() {
        auto strLs = FeederDecoder::Instance().AllAvalidMaterials();
        for (auto itr : m_unitMates)
        {
            auto str = itr.cmb->currentText();
            itr.cmb->clear();
            itr.cmb->addItems(strLs);
			if (!str.isEmpty())
				itr.cmb->setCurrentText(str);
			else
				strLs.removeAll(itr.cmb->currentText());
        }
        if (!m_unitMates.isEmpty())
            m_unitMates.last().spin->SetBtnVisible(strLs.size() > 0, 0);
    });
}

MaterialsSelect::~MaterialsSelect()
{
    delete m_ui;
}

void MaterialsSelect::GetFeedMaterials(QMap<QString, float>* mates)const
{
    if (mates)
    {
        auto& mts = *mates;
        for (auto &itr : m_unitMates)
        {
            auto str = itr.cmb->currentText();
            if (str.isEmpty())
                continue;
            mts[str] = itr.spin->GetWeight();
        }
    }
}

void MaterialsSelect::SetFeedMaterials(const FeederBottle *bt)
{
    if (bt)
    {
        m_ui->cmb_ch->setCurrentIndex(bt->getChannel());
        SpinCust* sp = NULL;
        QList<QPair<QString, float> > feederMats;
        bt->getFeedNameAndWeight(&feederMats);
        for (auto &itr : feederMats)
        {
            if (NULL == sp)
                sp = m_unitMates.last().spin;
            else
                addUnit(sp);
            m_unitMates.last().cmb->setCurrentText(itr.first);
            m_unitMates.last().spin->SetWeight(itr.second);
        }
    }
    m_ui->widget->setVisible(true);
}

int8_t MaterialsSelect::GetTubeNumber()const
{
    return m_ui->cmb_tube->currentIndex()-1;
}

int MaterialsSelect::GetChannel() const
{
    return m_ui->cmb_ch->currentIndex();
}

void MaterialsSelect::initUi(QComboBox* cmb, SpinCust* sp)
{
    auto strLs = FeederDecoder::Instance().AllAvalidMaterials();
    if (m_unitMates.size() == 0)
    { 
        sp->SetBtnVisible(false, 1);
    }
    else
    {
        if (m_unitMates.size() == 1)
            m_unitMates.first().spin->SetBtnVisible(true, 1);
        m_unitMates.last().mask->show();
    }

    for (auto& itr : m_unitMates)
    {
        strLs.removeAll(itr.cmb->currentText());
    }
    cmb->setFocusPolicy(Qt::NoFocus);
    cmb->addItems(strLs);
    cmb->setCurrentIndex(0);
    connect(sp, &SpinCust::btnClick, this, [=](int idx) {
        0 == idx ? addUnit(sp) : delUnit(sp);
        });
    if (!m_unitMates.isEmpty())
        m_unitMates.last().spin->SetBtnVisible(false, 0);
    auto mask = new MaskWidget(cmb);
    mask->hide();

    m_unitMates << UnitMate{ cmb, sp, mask };
    m_unitMates.last().spin->SetBtnVisible(strLs.size() > 1, 0);
}

void MaterialsSelect::addUnit(SpinCust* sp)
{
    auto idx = indexUnit(sp) + 1;
    auto cmb = new QComboBox(this);
    QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(cmb->sizePolicy().hasHeightForWidth());
    cmb->setSizePolicy(sizePolicy);
    cmb->setMinimumSize(QSize(120, 30));
    cmb->setMaximumSize(QSize(16777215, 120));
    m_ui->gridLayout->addWidget(cmb, idx + 1, 0, 1, 1);

    auto w = new SpinCust(this);
    m_ui->gridLayout->addWidget(w, idx + 1, 1, 1, 1);
    updateGeometry();
    initUi(cmb, w);
}

void MaterialsSelect::delUnit(SpinCust* sp)
{
    auto idx = indexUnit(sp);
    if (idx >= m_unitMates.size() || idx < 0)
        return;

    auto strLs = FeederDecoder::Instance().AllAvalidMaterials();
    auto u = m_unitMates.at(idx);
    QString str = u.cmb->currentText();
    if (!str.isEmpty())
    {
        for (auto itr = m_unitMates.begin() + idx + 1; itr != m_unitMates.end(); ++itr)
        {
            itr->cmb->addItem(str);
        }
    }
    m_ui->gridLayout->removeWidget(u.cmb);
    delete u.cmb;
    m_ui->gridLayout->removeWidget(u.spin);
    delete u.spin;
    m_unitMates.removeAt(idx);
    for (auto itr = m_unitMates.begin() + idx; itr != m_unitMates.end(); ++itr)
    {
        m_ui->gridLayout->addWidget(itr->cmb, idx + 1, 0, 1, 1);
        m_ui->gridLayout->addWidget(itr->spin, idx++ + 1, 1, 1, 1);
    }
    if (!m_unitMates.isEmpty())
    {
        if (m_unitMates.size() == 1)
            m_unitMates.first().spin->SetBtnVisible(false, 1);

        m_unitMates.last().spin->SetBtnVisible(true);
        m_unitMates.last().mask->hide();
    }
    updateGeometry();
}

int MaterialsSelect::indexUnit(SpinCust* sp)
{
    int ret = 0;
    for (const auto& itr : m_unitMates)
    {
        if (itr.spin == sp)
            return ret;
        ret++;
    }
    return ret;
}