#ifndef QFUNLABEL_H
#define QFUNLABEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QSpacerItem>
#include <QMessageBox>
#include <QFile>
#include <QListView>

class QFunLabel : public QGroupBox
{
    Q_OBJECT
public:
    explicit QFunLabel(QWidget *parent = nullptr);
    QLineEdit *name;
    QPushButton *addBtn;
signals:
    void labelPanelAdd(QString str);
public slots:
    void addBtn_clicked(void);
};

#endif // QFUNLABEL_H
