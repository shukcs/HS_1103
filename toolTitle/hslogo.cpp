#include "hslogo.h"
#pragma execution_character_set("utf-8")


Hslogo::Hslogo(QWidget *parent) : QLabel(parent)
{
    state = false;
    this->setFixedSize(164,50);
    this->setStyleSheet("QLabel{background-image: url(:/toolTitle/image/logo.png);}");
    this->setFocusPolicy(Qt::StrongFocus);

  //  this->grabKeyboard();//必须添加，K 大写
//    QMovie *movie = new QMovie(":/toolTitle/image/testlogo.gif");
//    this->setMovie(movie);
//    movie->start();

    connect(&timer,&QTimer::timeout,this,&Hslogo::tim_slot);
}

void Hslogo::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
//    switch (event->key()) {
//        case Qt::Key_Control:
//            state = true;
//            break;
//        default:
//            QWidget::keyPressEvent(event);
//    }
}

void Hslogo::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
//    state = false;
//    QWidget::keyReleaseEvent(event);
}

void Hslogo::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(!timer.isActive())
    {
       timer.start(3000);
    }
//    if (event->button() == Qt::LeftButton) {
//        if(state)
//        {
//          emit logo_clicked();
//            qDebug()<<"hello";
//        }
//    }
//    else if (event->button() == Qt::RightButton) {

    //    }
}

void Hslogo::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    timer.stop();
}

void Hslogo::tim_slot()
{
    emit logo_clicked();
    timer.stop();
}
