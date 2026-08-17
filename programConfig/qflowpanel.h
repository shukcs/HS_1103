#ifndef QFLOWPANEL_H
#define QFLOWPANEL_H

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

class QFlowPanel : public QGroupBox
{
    Q_OBJECT
public:
    explicit QFlowPanel(QWidget *parent = nullptr);

    QComboBox *flowList;
    QComboBox *flow_sw_actionList;
    QLineEdit *flowInput;
    QLabel *unit;
    QPushButton *addBtn;
    QPushButton *addBtn2;

signals:
    void flowPanelAdd(QString str);

public slots:
    void addBtn_clicked(void);
    void addBtn2_clicked(void);
};

#endif // QFLOWPANEL_H
