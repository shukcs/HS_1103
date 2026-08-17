#ifndef STOVE_H
#define STOVE_H

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include "stovetable.h"
#include "common/mymessageBox.h"

namespace Ui {
class stove;
}

class stove : public QWidget
{
    Q_OBJECT

public:
    explicit stove(QWidget *parent = 0);
    ~stove();

    void setTitle(QString str);
    QString getTitleName();
    void setTime(QString str);
    void setTemp(QString str);
    void setStep(QString str);
    void autoState(bool run);
    void manStoveRise(int stove);
    void manStoveFall(int stove);
    //void setTableHeader(QStringList header);
    void setTableFormat(int col,int row); // 设置表格的列和行
    void Table_init(void);
    int get_col(void);
    int get_row(void);

    void setTabelTemp(int *temp,int *time);

signals:

    void StoveTempRise(QString cmd);
    void StoveTempFalling(QString cmd);
    void StoveDataSave(QString cmd);
    void stove_refresh(int who,int row,int col,float val);
    void stove_change(int who);
private slots:

    void on_tempRise_clicked();
    void on_tempFall_clicked();
    void on_dataSave_clicked();

private:
    Ui::stove *ui;
    QList<int> *data;
    bool autoRun = false;
    bool check_run(QString str);
};

#endif // STOVE_H
