#include "ProgmaMgr.h"
#include <QFile>
#include <QDataStream>
#include "materialFeeder/FeederActionItem.h"

uint16_t ProgmaMgr::s_seq = 0;
ProgmaMgr::ProgmaMgr(QObject *p):QObject(p)
{
}

ProgmaMgr::~ProgmaMgr()
{
	for (auto &itr : m_loads)
	{
		qDeleteAll(itr.second);
	}
	qDeleteAll(m_edits);
}

const QList<ProgmaMgr::LoadItem> & ProgmaMgr::AllLoadGroup() const
{
	return m_loads;
}

void ProgmaMgr::Reload()
{

}

bool ProgmaMgr::Save(const QString &file)
{
	QFile f(file);
	if (f.open(QIODevice::WriteOnly))
	{
		QDataStream st(&f);
		for (auto itr : m_edits)
		{
			itr->Save(&st);
		}
	}
	return false;
}

void ProgmaMgr::AddLabel(const QString &label)
{
	m_edits << new LabelItem(label);
}

void ProgmaMgr::AddSolidPrepare(uint16_t numBottle, uint16_t numTube, const QList<FeedItem> &weightFeeds)
{
	m_edits << new SolidPrepareItem(numBottle, numTube, weightFeeds, s_seq++);
}

ActionAbstrctItem * ProgmaMgr::create(uint8_t type)
{
	switch (ActionType(type))
	{
	case Act_Delay:
		break;
	case Act_LoopBeg:
		break;
	case Act_LoopEnd:
		break;
	case Act_CuverRecBeg:
		break;
	case Act_CuverRecEnd:
		break;
	case FL_Label:
		return new LabelItem();
	case Group_PrepareSolidMate:
		return new SolidPrepareItem();
	case Group_StoveFixTube:
		return new FixTubeItem();
	case Group_StoveTubeBack:
		return new TubeBackItem();
		break;
	case Group_AirClear:
		break;
	case Group_AirIn:
		break;
	case Group_AirEnd:
		break;
	case Group_LiquiClear:
		break;
	case Group_LiquiIn:
		break;
	case Group_LiquiEnd:
		break;
	case Group_StoveHeat:
		break;
	default:
		break;
	}
	return nullptr;
}
