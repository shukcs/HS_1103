#include "DemoWidget.h"
#include <QPainter>
#include "materialFeeder/FeederDecoder.h"

DemoWidget::DemoWidget(QWidget *p) : QWidget(p)
, m_bg(new QPixmap(":/image/balance.png"))
{
    connect(&FeederDecoder::Instance(), &FeederDecoder::containerChanged, this, [=](const StoreStruct *st) {
        if (st->stat == 2)
            m_feeding = st;
        bool b = m_feeding && 2==m_feeding->stat;
        if (b != m_bFeeding)
        {
            m_bFeeding = b;
            update();
        }
    });
}

DemoWidget::~DemoWidget()
{
    delete m_bg;
}

void DemoWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    drawBalance(&p);
    if (m_bFeeding)
    {
        drawStore(&p);
        drawFeeding(&p);
    }
}

void DemoWidget::drawBalance(QPainter *p)
{
    QRect rc(paintRect().topLeft() + QPoint(81, 158), m_bg->size());
    p->drawPixmap(rc, *m_bg);
}

void DemoWidget::drawFeeding(QPainter *p)
{
    static QPixmap pix(":/image/feeding.png");
    QRect rc(paintRect().topLeft() + QPoint(140, 127), pix.size());
    p->drawPixmap(rc, pix);
}

void DemoWidget::drawStore(QPainter *p)
{
    static QPixmap pix(":/image/store_b.png");
    QRect rc(paintRect().topLeft() + QPoint(120, 0), pix.size());
    if (m_feeding && m_feeding->pMate)
    {
        p->drawText(QRect(rc.topLeft() + QPoint(5, 47), QSize(41, 14)), QString::number(m_feeding->numb));
        p->drawText(QRect(rc.topLeft() + QPoint(5, 61), QSize(41, 14)), m_feeding->pMate->name);
        p->drawText(QRect(rc.topLeft() + QPoint(5, 75), QSize(41, 14)), QString("%1¿Ë").arg(m_feeding->pMate->weight));
    }
    p->drawPixmap(rc, pix);
}

QRect DemoWidget::paintRect() const
{
    QRect rc = QRect(QPoint(0, 0), QSize(328, 346));
    rc.moveCenter(rect().center());
    return rc;
}
