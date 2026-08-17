#ifndef QVALVECTRL_H
#define QVALVECTRL_H

#include <QGroupBox>


class QLineEdit;
class QLabel;
class QPushButton;
class QComboBox;
class QValveCtrl : public QGroupBox
{
    Q_OBJECT
public:
    explicit QValveCtrl(QWidget *parent = nullptr);

    QLineEdit *pres;
    QLabel *name;
    QLabel *unit;
    QPushButton *addBtn;

    QComboBox *valveList;
signals:
    void valvePanelAdd(QString str);
public slots:
    void addBtn_clicked(void);
};

#endif // QVALVECTRL_H
