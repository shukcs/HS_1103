#include "MaterialTableWidget.h"
#include <QScrollBar>
#include <QEvent>
#include "../FeederMgr.h"

MaterialTableWidget::MaterialTableWidget(QWidget* p) : QTableWidget(p)
{
    if (auto bar = verticalScrollBar())
        bar->installEventFilter(this);
}

void MaterialTableWidget::AddMaterial(const MaterialStruct &m)
{
    if (m.nfcid.isEmpty())
        return; 

    auto idx = rowCount();
    insertRow(idx);

    setItem(idx, 0, new QTableWidgetItem(QString::number(idx + 1)));
    setItem(idx, 1, new QTableWidgetItem(m.nfcid));
    setItem(idx, 2, new QTableWidgetItem(m.name));
    setItem(idx, 3, new QTableWidgetItem(QString::number(m.weight)));
    setItem(idx, 4, new QTableWidgetItem(QString("%1/%2/%3").arg(m.data.year+2000).arg(m.data.month).arg(m.data.day)));
}

void MaterialTableWidget::ChangeMaterial(const QString& id, const MaterialStruct& m)
{
    if (id.isEmpty())
        return;

    auto idx = findIdRow(id);
    if (idx >= 0)
    {
        setItem(idx, 0, new QTableWidgetItem(QString::number(idx + 1)));
        setItem(idx, 1, new QTableWidgetItem(m.nfcid));
        setItem(idx, 2, new QTableWidgetItem(m.name));
        setItem(idx, 3, new QTableWidgetItem(QString::number(m.weight)));
    }
}

bool MaterialTableWidget::CurrentRowMaterial(MaterialStruct *m)
{
    auto idx = currentRow();
    if (idx < 0)
        return false;

    if (auto it = item(idx, 1))
        m->nfcid = it->text();
    if (auto it = item(idx, 2))
        m->name = it->text();
    if (auto it = item(idx, 3))
        m->weight = it->text().toDouble();

    return true;
}

bool MaterialTableWidget::eventFilter(QObject* obj, QEvent* e)
{
    static QSet<QEvent::Type> typesFilet = { QEvent::ShowToParent, QEvent::HideToParent };
    if (obj == verticalScrollBar() && typesFilet.contains(e->type()))
    {
        auto w = verticalScrollBar()->sizeHint().width();
        if (e->type() == QEvent::HideToParent)
            setFixedWidth(width() - w);
        else if (QEvent::ShowToParent)
            setFixedWidth(width() + w);
    }
    return QTableWidget::eventFilter(obj, e);
}

int MaterialTableWidget::findIdRow(const QString& id)
{
    for (int i = 0; i < rowCount(); ++i)
    {
        if (auto it = item(i, 1))
        {
            if (it->text() == id)
                return i;
        }
    }
    return -1;
}