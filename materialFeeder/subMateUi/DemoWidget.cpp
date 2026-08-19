#include "DemoWidget.h"
#include <QPainter>
#include <QSet>
#include "materialFeeder/FeederDecoder.h"
#pragma execution_character_set("utf-8")

DemoWidget::DemoWidget(QWidget *p) : QWidget(p)
, m_bg(new QPixmap(":/image/balance.png"))
{
    connect(&FeederMgr::Instance(), &FeederMgr::storeChanged, this, [=](const StoreStruct *st) {
		auto stat = st->getStat();
        static QSet<uint8_t> sSet = {S_Feeding, S_WaitFeed};
        bool bOld = sSet.contains(m_statStore);
        bool bNow = sSet.contains(stat);
        if (m_feeding==st && stat!=m_statStore)
        {
            m_statStore = stat;
            if (bOld || bNow)
                update();
            return;
        }

        if (bNow)
			m_feeding = st;

        if (bNow || m_statStore != stat)
        {
			m_statStore = stat;
            update();
        }
    });
    connect(&FeederMgr::Instance(), &FeederMgr::feedingChanged, this, [=](float w, bool b) {
        if (b)
        {
            m_wFeed = w;
            update();
        }
        else if(!FeederMgr::Equal(w, m_wFeeding))
        {
            m_wFeeding = w;
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
	if (S_WaitFeed == m_statStore)
		drawStore(&p);
    else if (S_Feeding == m_statStore)
        drawFeeding(&p);
}

void DemoWidget::drawBalance(QPainter *p)
{
    QRect rc(paintRect().topLeft() + QPoint(81, 158), m_bg->size());
    p->drawPixmap(rc, *m_bg);
    if (S_Feeding == m_statStore)
        p->drawText(rc, Qt::AlignCenter, tr("进料：%1").arg(m_wFeeding));
}

void DemoWidget::drawFeeding(QPainter *p)
{
	drawStore(p);

    static QPixmap pix(":/image/feeding.png");
    QRect rc(paintRect().topLeft() + QPoint(140, 127), pix.size());
    p->drawPixmap(rc, pix);
}

void DemoWidget::drawStore(QPainter *p)
{
    static QPixmap pix(":/image/store_b.png");
    QRect rc(paintRect().topLeft() + QPoint(120, 0), pix.size());
    if (m_feeding)
    {
        p->drawText(QRect(rc.topLeft() + QPoint(5, 47), QSize(41, 14)), Qt::AlignCenter, QString::number(m_feeding->numb));
        p->drawText(QRect(rc.topLeft() + QPoint(5, 61), QSize(41, 14)), Qt::AlignCenter, tr("投料"));
        p->drawText(QRect(rc.topLeft() + QPoint(5, 75), QSize(41, 14)), Qt::AlignCenter, QString("%1克").arg(m_wFeed));
    }
    p->drawPixmap(rc, pix);
}

QRect DemoWidget::paintRect() const
{
    QRect rc = QRect(QPoint(0, 0), QSize(328, 346));
    rc.moveCenter(rect().center());
    return rc;
}
