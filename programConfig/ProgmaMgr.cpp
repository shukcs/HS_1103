#include "ProgmaMgr.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDataStream>
#include <QApplication>
#include "common/ActionFactory.h"
#include "common/ActionItem.h"

uint16_t ProgmaMgr::s_seq = 0;
ProgmaMgr::ProgmaMgr(QObject *p):QObject(p)
{
}

ProgmaMgr::~ProgmaMgr()
{
    clearLoads();
	qDeleteAll(m_edits);
}

const QList<ProgmaMgr::LoadItem> & ProgmaMgr::AllLoadGroup() const
{
	return m_loads;
}

const ProgmaMgr::ActionsGroup &ProgmaMgr::EditActionsGroup() const
{
    return m_edits;
}

void ProgmaMgr::Reload(const QString &path)
{
    clearLoads();
    QDir dir(path);
    for (auto &itr : dir.entryInfoList({ "*.prg" }))
    {
        QFile f(itr.absoluteFilePath());
        if (f.open(QIODevice::ReadOnly))
        {
            ActionsGroup gp;
            QDataStream st(&f);
            load(&st, gp);
            m_loads << LoadItem(itr.baseName(), gp);
        }
    }
    emit loadsRefrashed();
}

void ProgmaMgr::Load(const QString &file)
{
    QFile f(file);
    if (f.open(QIODevice::ReadOnly))
    {
        qDeleteAll(m_edits);
        m_edits.clear();
        QDataStream st(&f);
        load(&st, m_edits);
        if (!m_edits.isEmpty())
            s_seq = m_edits.last()->GetSeq() + 1;
    }
    emit curActionsGroupChanged();
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
	addItem(new LabelItem(label));
}

void ProgmaMgr::AddAction(ActionAbstrctItem *act)
{
    addItem(act);
}

void ProgmaMgr::RemoveAt(int idx)
{
    if (0 <= idx && idx < m_edits.size())
    {
        delete m_edits.takeAt(idx);
        emit itemRemovd(idx);
    }
}

void ProgmaMgr::MoveUpAt(int idx)
{
    if (0 < idx && idx < m_edits.size())
    {
        auto act = m_edits.takeAt(idx);
        m_edits.insert(idx - 1, act);
        emit itemMoveUp(idx);
    }
}

void ProgmaMgr::MoveDownAt(int idx)
{
    if (0 <= idx && idx+1 < m_edits.size())
    {
        auto act = m_edits.takeAt(idx);
        m_edits.insert(idx + 1, act);
        emit itemMoveDown(idx);
    }
}

ProgmaMgr &ProgmaMgr::Instance()
{
    static  ProgmaMgr sIns(nullptr);
    return sIns;
}

void ProgmaMgr::load(QDataStream *ds, ActionsGroup &f)
{
    if (!ds)
        return;

    int8_t t;
    while (!ds->atEnd())
    {
        *ds >> *(int8_t*)&t;
        if (auto item = ActionAbsFacItem::GetActionByType(t))
        {
            item->Load(ds);
            f << item;
        }
    }
}

void ProgmaMgr::clearLoads()
{
    for (auto &itr : m_loads)
    {
        qDeleteAll(itr.second);
    }
    m_loads.clear();
}

void ProgmaMgr::addItem(ActionAbstrctItem *item)
{
    if (!item)
        return;

    m_edits << item;
    emit actionItemAdded(item);
    if (item->getType() != FL_Label)
    {
        item->m_seq = s_seq++;
        if (s_seq > 0x7fff)
            s_seq = 0;
    }
}
