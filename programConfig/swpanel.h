#ifndef SWPANEL_H
#define SWPANEL_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QDebug>
#include <QFile>
#include <QListView>

class SwPanel : public QGroupBox
{
    Q_OBJECT
public:
    explicit SwPanel(QWidget *parent = nullptr);

    QComboBox *swList;
    QComboBox *actionList;
    QPushButton *addBtn;

signals:
    void swPanelAdd(QString str);

public slots:

    void addBtn_clicked(void);
};

#endif // SWPANEL_H
