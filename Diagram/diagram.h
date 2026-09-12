#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QWidget>
#include "strDecoder/portthread.h"

#define SW_NUM      6
#define ACTSW_NUM   2
#define SEC_SW      1       //开关阀
#define FOR_SW      2       //四通阀

namespace Ui {
class diagram;
}

class flowSet;
class diagram : public QWidget
{
    Q_OBJECT

public:
    explicit diagram(QWidget *parent = 0);
    ~diagram();

    void updateInfo(ReceiveData *data);
    void strToState(const QString& cmd);   // 调整开关阀图标到相应状态
    void autoRun(bool state);       // 更新运行模式
    void swIconCtrl(int id,bool state);
    void updateHeatAndKeep(int idx, float tmp);
    void updateTriEleValveStat(int idx, int pos, bool bReached);
signals:
    void cmdTorun(const QString& str);

private slots:
    void on_sw1_clicked();
    void on_sw1_2_clicked();
    void on_sw1_3_clicked();
    void on_sw2_clicked();
    void on_sw2_2_clicked();
    void on_sw2_3_clicked();

    void on_flow1set_clicked();
    void on_flow2set_clicked();
    void on_pump1set_clicked();
    void on_pump2set_clicked();
    void on_pres1set_clicked();
	void on_pres2set_clicked();
	void on_pres1_min_clicked();
    void on_pres2_min_clicked();
    void on_btn_rate1_clicked();
    void on_btn_rate2_clicked();

    void flow_set_slot(QString str);
private:
    bool check_run(const QString& str);
    void initSlots();
    flowSet *createFlowSet(const QString& title, int id, const QString& name, const QVariant &v, const QString &unit=QString());
    void setSw(int idx, bool b);
private:
    Ui::diagram *ui;
    bool auto_Run = false;
    bool update_flg = true;
    bool flow_sw[FLOW_NUM];     /// 流量计阀控
    bool swState[SW_NUM];
    double flow_set[FLOW_NUM];  /// 设定的流量
    double pump_flow[PUMP_NUM]; /// 液体泵设定流量
    double pres_set[4];         /// 设定的压力
    double m_percentLiquid[2];  ///液位
    uint8_t m_posValve3Way[2];  /// 3通阀通向
};

#endif // DIAGRAM_H
