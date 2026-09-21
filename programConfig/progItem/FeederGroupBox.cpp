#include "FeederGroupBox.h"
#include "common/mymessageBox.h"
#include "strDecoder/strdecoder.h"
#include "materialFeeder/FeederMgr.h"
#include "materialFeeder/FeederActionItem.h"

#include "Ui_FeederGroupBox.h"
#pragma execution_character_set("utf-8")

FeederGroupBox::FeederGroupBox(QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::FeederGroupBox)
{
    m_ui->setupUi(this);
    initUi();
}

FeederGroupBox::~FeederGroupBox()
{
    delete m_ui;
}

void FeederGroupBox::initUi()
{
	m_ui->pushButton->setEnabled(false);
    m_ui->widget->SetFeedMaterials(nullptr);
    m_ui->widget->SetBottleSelected(true);

	connect(m_ui->pushButton, &QPushButton::clicked, this, &FeederGroupBox::onAdd);
    connect(m_ui->btn_tubeback, &QPushButton::clicked, this, &FeederGroupBox::onTubeBack);
    connect(m_ui->btn_fix, &QPushButton::clicked, this, &FeederGroupBox::onFix);
    connect(m_ui->widget, &MaterialsSelect::avalidChanged, this, [=](bool b) { m_ui->pushButton->setEnabled(b); });
    for (auto itr : FeederMgr::Instance().AllTubes())
    {
        if (FeederMgr::Instance().CanAddWork(Group_StoveFixTube, itr->getNumber(), true))
            m_ui->cmb_tube->addItem(tr("反应管%1").arg(itr->getNumber()+1));
    }

    connect(&FeederMgr::Instance(), &FeederMgr::tubeChanged, this, [=](const TubeStruct *tb) {
        auto str = tr("反应管%1").arg(tb->getNumber()+1);
        if (FeederMgr::Instance().CanAddWork(Group_StoveFixTube, tb->getNumber(), true))
        {
            for (int i = 0; i < m_ui->cmb_tube->count(); ++i)
            {
                auto cur = m_ui->cmb_tube->itemText(i).remove(tr("反应管")).toInt();
                if (cur >= tb->getNumber() + 1 || i+1==m_ui->cmb_tube->count())
                {
                    if (cur == tb->getNumber() + 1)
                        break;
                    else if (cur > tb->getNumber() + 1)
                        m_ui->cmb_tube->insertItem(i, str);
                    else
                        m_ui->cmb_tube->insertItem(i+1, str);
                    break;
                }
            }
        }
        else
        {
            auto idx = m_ui->cmb_tube->findText(str);
            if (idx >= 0)
                m_ui->cmb_tube->removeItem(idx);
        }
    });
}

void FeederGroupBox::addTube(const TubeStruct* tube)
{
    if (!tube)
        return;
}

void FeederGroupBox::onAdd()
{
    QList<QPair<QString, float>> feeds;
    m_ui->widget->GetFeedMaterials(&feeds);
    auto tmpB = m_ui->widget->GetSelectedBottleNum();
    auto tmpT = m_ui->widget->GetTubeNumber();
    if (auto act = new SolidPrepareItem(tmpB, tmpT, feeds))
        act->AddToEdit();
}

void FeederGroupBox::onTubeBack()
{
    auto ch = m_ui->comboBox->currentIndex();
    if (auto act = new TubeBackItem(ch, 1))
        act->AddToEdit();
}

void FeederGroupBox::onFix()
{
    auto ch = m_ui->cmb_fix->currentIndex();
    auto nTube = m_ui->cmb_tube->currentIndex();
    if (auto act = new FixTubeItem(ch, nTube))
        act->AddToEdit();
}
