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
    SetBottleSelected(false);
    initUi(m_ui->cmb_id, m_ui->w_id);
    changeAvalidTube();
    changeAvalidBottle();
    connect(&FeederMgr::Instance(), &FeederMgr::matesCanFeedChanged, this, &MaterialsSelect::changeAvalidMate);
    connect(&FeederMgr::Instance(), &FeederMgr::canUsedTubeChanged, this, &MaterialsSelect::changeAvalidTube);
    connect(&FeederMgr::Instance(), &FeederMgr::canUsedBottleChanged, this, &MaterialsSelect::changeAvalidBottle);
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

void MaterialsSelect::SetFeedMaterials(const FeederParam *bt)
{
    if (auto tb = bt ? bt->getTube() : nullptr)
    {
        m_ui->cmb_ch->setCurrentIndex(tb->getStoveCh());
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
    return m_ui->cmb_tube->currentText().remove(tr("反应管")).toInt()-1;
}

int MaterialsSelect::GetChannel() const
{
	auto idx = m_ui->cmb_ch->currentIndex();
	return idx == m_ui->cmb_ch->count() - 1 ? -1 : idx;;
}

void MaterialsSelect::SetBottleSelected(bool b)
{
    m_ui->stackedWidget->setCurrentWidget(b ? m_ui->p_bottle : m_ui->p_stove);
}

int MaterialsSelect::GetSelectedBottleNum() const
{
    return m_ui->cmb_bottle->currentText().remove(tr("料瓶")).toInt()-1;
}

void MaterialsSelect::initUi(QComboBox* cmb, SpinCust* sp)
{
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

    cmb->setFocusPolicy(Qt::NoFocus);
    connect(sp, &SpinCust::btnClick, this, [=](int idx) {
        0 == idx ? addUnit(sp) : delUnit(sp);
        });
    if (!m_unitMates.isEmpty())
        m_unitMates.last().spin->SetBtnVisible(false, 0);
    auto mask = new MaskWidget(cmb);
    mask->hide();

    auto strLs = FeederMgr::Instance().AllAvalidMaterials();
    for (auto& itr : m_unitMates)
    {
        strLs.removeAll(itr.cmb->currentText());
    }
    cmb->addItems(strLs);
    cmb->setCurrentIndex(0);
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

    auto strLs = FeederMgr::Instance().AllAvalidMaterials();
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

void MaterialsSelect::changeAvalidMate()
{
    auto strLs = FeederMgr::Instance().AllAvalidMaterials();
    if (m_bVallid && strLs.isEmpty())
    {
        m_bVallid = false;
        emit avalidChanged(m_bVallid);
    }
    else if (!m_bVallid && !strLs.isEmpty() && !FeederMgr::Instance().ValidBottls().isEmpty() && !FeederMgr::Instance().ValidTubes().isEmpty())
    {
        m_bVallid = true;
        emit avalidChanged(m_bVallid);
    }
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
}

void MaterialsSelect::changeAvalidTube()
{
    auto vts = FeederMgr::Instance().ValidTubes();
    if (m_bVallid && vts.isEmpty())
    {
        m_bVallid = false;
        emit avalidChanged(m_bVallid);
    }
    else if (!m_bVallid && !vts.isEmpty() && !FeederMgr::Instance().ValidBottls().isEmpty() && !FeederMgr::Instance().AllAvalidMaterials().isEmpty())
    {
        m_bVallid = true;
        emit avalidChanged(m_bVallid);
    }
    auto str = m_ui->cmb_tube->currentText();
    m_ui->cmb_tube->clear();
    m_ui->cmb_tube->addItem(tr("自动"));
    for (auto itr : vts)
    {
        m_ui->cmb_tube->addItem(tr("反应管%1").arg(itr->getNumber() + 1));
    }
    if (!str.isEmpty())
        m_ui->cmb_tube->setCurrentText(str);
}

void MaterialsSelect::changeAvalidBottle()
{
    auto vbs = FeederMgr::Instance().ValidBottls();
    if (m_bVallid && vbs.isEmpty())
    {
        m_bVallid = false;
        emit avalidChanged(m_bVallid);
    }
    else if (!m_bVallid && !vbs.isEmpty() && !FeederMgr::Instance().ValidTubes().isEmpty() && !FeederMgr::Instance().AllAvalidMaterials().isEmpty())
    {
        m_bVallid = true;
        emit avalidChanged(m_bVallid);
    }
    auto str = m_ui->cmb_bottle->currentText();
    m_ui->cmb_bottle->clear();
    m_ui->cmb_bottle->addItem(tr("自动"));
    for (auto itr : vbs)
    {
        m_ui->cmb_bottle->addItem(tr("料瓶%1").arg(itr->m_numb + 1));
    }
    if (!str.isEmpty())
        m_ui->cmb_bottle->setCurrentText(str);
}
