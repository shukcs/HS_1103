#include "FeederGroupBox.h"
#include "common/mymessageBox.h"
#include "strDecoder/strdecoder.h"
#include "materialFeeder/FeederMgr.h"

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

QString FeederGroupBox::_getFixStr() const
{
    return QString(" %1 %2 %3 %4").arg(m_ui->lb_feed->text()).arg(m_ui->cmb_fix->currentIndex()+1)
		.arg(m_ui->lb_tube->text()).arg(m_ui->cmb_tube->currentIndex() + 1);
}

QString FeederGroupBox::_getBackStr()const
{
	return QString(" %1 %2").arg(m_ui->lb_back->text()).arg(m_ui->comboBox->currentIndex()+1);
}

QString FeederGroupBox::_getFeedMateStr()const
{
	QMap<QString, float> feeds;
	m_ui->widget->GetFeedMaterials(&feeds);
    auto tmp = m_ui->widget->GetSelectedBottleNum();
    QString strBottle = tmp < 0 ? tr("自动") : QString::number(tmp + 1);
    tmp = m_ui->widget->GetTubeNumber();
    auto strTube = tmp < 0 ? tr("自动") : QString::number(tmp + 1);
	QString ret = tr(" 配料 料瓶 %1 反应管 %2 ").arg(strBottle).arg(strTube);
	for (auto itr=feeds.begin(); itr != feeds.end(); ++itr)
	{
		ret += itr.key();
		ret += " " + QString::number(itr.value());
	}
	return ret;
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
        if (FeederMgr::Instance().CanAddWork(FeederMgr::J_StoveFixTube, itr->getNumber(), true))
            m_ui->cmb_tube->addItem(tr("反应管%1").arg(itr->getNumber()+1));
    }

    connect(&FeederMgr::Instance(), &FeederMgr::tubeChanged, this, [=](const TubeStruct *tb) {
        auto str = tr("反应管%1").arg(tb->getNumber()+1);
        if (FeederMgr::Instance().CanAddWork(FeederMgr::J_StoveFixTube, tb->getNumber(), true))
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
    emit sig_Add(title() + _getFeedMateStr());
}

void FeederGroupBox::onTubeBack()
{
	emit sig_Add(title() + _getBackStr());
}

void FeederGroupBox::onFix()
{
    emit sig_Add(title() + _getFixStr());
}
