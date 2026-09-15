#ifndef QCOLLECTORCTRL_H
#define QCOLLECTORCTRL_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QListView>
#include "common/mymessageBox.h"

class QCollectorCtrl : public QGroupBox
{
    Q_OBJECT
public:
    explicit QCollectorCtrl(QWidget *parent = nullptr);

    QLineEdit *sample[3];
    QLabel *unit[3];
    QPushButton *addBtn;
    QPushButton *helpBtn;       // 使用说明

    QComboBox *collList;
    QComboBox *actList;
signals:
    void collPanelAdd(QString str);
public slots:
    void addBtn_clicked(void);
    void helpBtn_clicked(void);

//private:
//    bool flag = false;
};

#endif // QCOLLECTORCTRL_H
