#ifndef HSLOGO_H
#define HSLOGO_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QMovie>
#include <QTimer>

class Hslogo : public QLabel
{
    Q_OBJECT
public:
    explicit Hslogo(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void logo_clicked(void);
public slots:
    void tim_slot(void);
private:
    bool state;

    QTimer timer;
};

#endif // HSLOGO_H
