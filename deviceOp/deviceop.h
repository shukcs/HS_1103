#ifndef DEVICEOP_H
#define DEVICEOP_H

#include <QWidget>
#include <QFile>
#include <QDir>
#include <QValidator>
#include <QTextStream>
#include "common/mymessageBox.h"
#include "strDecoder/portthread.h"


namespace Ui {
class deviceOp;
}

class deviceOp : public QWidget
{
    Q_OBJECT

public:
    explicit deviceOp(QWidget *parent = 0);
    ~deviceOp();
    void updateInfo(ReceiveData *data);

signals:
    void cmdTorun(const QString &);
    void selfcmdTorun(const QString&);

public slots:
    void update_user_set(bool state);
    void autoRun(bool state);       // 运行状态更新
    void tempAdjustEnable(void);
    void readyTorun(const QString&);
    void stove_refresh(int who,int row,int col,float val);

private slots:
//    void on_temp1_editingFinished();
//    void on_temp2_editingFinished();
//    void on_temp3_editingFinished();
//    void on_temp4_editingFinished();
//    void on_temp5_editingFinished();
//    void on_temp6_editingFinished();
//    void on_temp7_editingFinished();
//    void on_temp8_editingFinished();
//    void on_temp9_editingFinished();
//    void on_temp10_editingFinished();

//    void on_time1_editingFinished();
//    void on_time2_editingFinished();
//    void on_time3_editingFinished();
//    void on_time4_editingFinished();
//    void on_time5_editingFinished();
//    void on_time6_editingFinished();
//    void on_time7_editingFinished();
//    void on_time8_editingFinished();
//    void on_time9_editingFinished();
//    void on_time10_editingFinished();

//    void on_run_clicked();
//    void on_stop_clicked();
//    void on_save_clicked();

    void on_temp1Adj_btn_clicked();

    void on_pres_auto_clicked();
    void on_pres_auto_2_clicked();

    void on_flow1_sw_clicked();
    void on_flow2_sw_clicked();
    void on_flow1btn_clicked();
    void on_flow2btn_clicked();

    void on_pump_clean_clicked();
    void on_pump_clean_2_clicked();
    void on_pump_run_clicked();
    void on_pump_run_2_clicked();

    void on_pres_inc_pressed();
    void on_pres_inc_2_pressed();

    void on_pres_inc_released();
    void on_pres_inc_2_released();

    void on_pres_dec_pressed();
    void on_pres_dec_2_pressed();

    void on_pres_dec_released();
    void on_pres_dec_2_released();

    void on_pres_on_clicked();
    void on_pres_on_2_clicked();

    void on_pres_off_clicked();
    void on_pres_off_2_clicked();

//    void on_flow1_range_editingFinished();
//    void on_flow2_range_editingFinished();

//    void on_pump_cali_editingFinished();
//    void on_pump_cali_2_editingFinished();

    bool check_run(QString str);

private:
    Ui::deviceOp *ui;
    int stove_No = 0;
    bool user_state = false;
    bool auto_Run = false;

};

#endif // DEVICEOP_H
