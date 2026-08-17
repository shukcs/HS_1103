#include "FeederGroupBox.h"
#include "materialFeeder/FeederDecoder.h"
#include "common/mymessageBox.h"
#include "strDecoder/strdecoder.h"

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

QString FeederGroupBox::_getBackStr()const
{
	return QString(" %1 %2").arg(m_ui->lb_back->text()).arg(m_ui->comboBox->currentIndex());
}

QString FeederGroupBox::_getFeefChStr()const
{
    return QString(" %1 %2").arg(m_ui->lb_feed->text()).arg(m_ui->comboBox->currentIndex());
}

QString FeederGroupBox::_getFeefMateStr()const
{
	QMap<QString, float> feeds;
	m_ui->widget->GetFeedMaterials(&feeds);
	QString ret(" ");
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
	connect(&FeederDecoder::Instance(), &FeederDecoder::matesCanFeedChanged, this, [=]() {
		m_ui->pushButton->setEnabled(!FeederDecoder::Instance().AllAvalidMaterials().isEmpty());
	});

	connect(m_ui->pushButton, &QPushButton::clicked, this, &FeederGroupBox::onAdd);
	connect(m_ui->btn_tubeback, &QPushButton::clicked, this, &FeederGroupBox::onTubeBack);
}

void FeederGroupBox::addTube(const TubeStruct* tube)
{
    if (!tube)
        return;
}

void FeederGroupBox::onAdd()
{
    emit sig_Add(title() + _getFeefChStr() + _getFeefMateStr());
}

void FeederGroupBox::onTubeBack()
{
	emit sig_Add(title() + _getBackStr());
}
