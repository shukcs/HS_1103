#ifndef __DemoWidget__H__
#define __DemoWidget__H__
#include <QWidget>

class QPixmap;
class StoreStruct;
class DemoWidget : public QWidget
{
public:
    DemoWidget(QWidget *p=nullptr);
    ~DemoWidget();
protected:
    void paintEvent(QPaintEvent *e);

    void drawBalance(QPainter *p);
    void drawFeeding(QPainter *p);
    void drawStore(QPainter *p);

    QRect paintRect()const;
private:
    QPixmap *m_bg;
    const StoreStruct *m_feeding=nullptr;
    bool  m_bFeeding = false;
};

#endif