#include "MaskWidget.h"
#include <qEvent.h>

MaskWidget::MaskWidget(QWidget *w) : QWidget(w)
{
    if (w)
    {
        w->installEventFilter(this);
        setGeometry(QRect(QPoint(0, 0), w->size()));
    }
    setStyleSheet("background-color: transparent;");
}

bool MaskWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (obj==parent() && e->type()==QEvent::Resize)
    {
        auto rc = QRect(QPoint(0, 0), ((QResizeEvent*)e)->size());
        setGeometry(rc);   //蒙住所有
        raise();           // 提升到最上层
    }
    return QObject::eventFilter(obj, e);
}

bool MaskWidget::event(QEvent *e)
{
    static QSet<QEvent::Type> filters = { QEvent::MouseButtonPress, QEnterEvent::MouseButtonRelease,QEnterEvent::MouseButtonDblClick, QEnterEvent::Wheel };
    if (filters.contains(e->type()))
    {
        e->accept();
        return true; // 拦截鼠标
    }

    return QWidget::event(e);
}