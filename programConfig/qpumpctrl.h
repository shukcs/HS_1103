#ifndef QPUMPCTRL_H
#define QPUMPCTRL_H

#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class QPumpCtrl : public QGroupBox
{
    Q_OBJECT
public:
    explicit QPumpCtrl(QWidget *parent = nullptr);

    QLineEdit *speed;
    QLabel *unit;
    QPushButton *addBtn;
    QPushButton *addBtn2;
    QPushButton *helpBtn;       // 使用说明

    QComboBox *pumpList;
    QComboBox *actList;
signals:
    void pumpPanelAdd(QString str);
public slots:
    void addBtn_clicked(void);
    void addBtn2_clicked(void);
    void helpBtn_clicked(void);
};

#endif // QPUMPCTRL_H
