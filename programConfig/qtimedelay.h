#ifndef QTIMEDELAY_H
#define QTIMEDELAY_H

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>


class QTimeDelay : public QGroupBox
{
    Q_OBJECT
public:
    explicit QTimeDelay(QWidget *parent = nullptr);

    QLineEdit *timeInput;
    QLabel *unit;
    QPushButton *addBtn;

signals:
    void delayPanelAdd(QString str);

public slots:
    void addBtn_clicked(void);
};

#endif // QTIMEDELAY_H
