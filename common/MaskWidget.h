#ifndef __MASKWIDGET_H__
#define __MASKWIDGET_H__

#include <QWidget>

class MaskWidget : public QWidget {
public:
    MaskWidget(QWidget *w);
protected:
    bool eventFilter(QObject *w, QEvent *e)override;
    bool event(QEvent *e)override;
};

#endif  // __MASKWIDGET_H__
