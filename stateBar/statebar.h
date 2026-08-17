#ifndef STATEBAR_H
#define STATEBAR_H

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QComboBox>
#include <QFile>
#include <QListView>
#include <QDir>
#include <QByteArray>

class portName
{
public:
    QString name;
    portName()
    {
      name = "";
    }
};

namespace Ui {
class stateBar;
}

class stateBar : public QWidget
{
    Q_OBJECT

public:
    explicit stateBar(QWidget *parent = 0);
    ~stateBar();
    portName port;

signals:
    void sampleTimeChanged(int time);
    void portChanged(QString name);
    void autoBtn_clicked();
    void manualBtn_clicked();
    void collectorBtn_clicked();
    void graphBtn_clicked();
    void logoBtn_clicked();
    void storeBtn_clicked();
private slots:
    void setConnectionState(bool state);
    void on_sampleTime_currentTextChanged(const QString &arg1);
    void on_connectState_clicked();

    void on_autoBtn_clicked();
    void on_manualBtn_clicked();
    void on_collectorBtn_clicked();
    void on_graphBtn_clicked();

private:
    Ui::stateBar *ui;
    QFile file;
};

#endif // STATEBAR_H
