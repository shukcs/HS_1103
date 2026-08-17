#ifndef CUSTOMGRAPH_H
#define CUSTOMGRAPH_H

#include <QWidget>
#include <QTimer>

#define LINE_NUM 12

class HSGraphData
{
public:
       int x;
       double temp;  // 温度
       QVector<double> x_val, y_val;

       HSGraphData()
       {
           x = 0;
           temp = 0;
       }
       void clear()
       {
           x = 0;
           temp = 0;
           x_val.clear();
           y_val.clear();
       }
};


class QCPAxis;
class QCPItemText;
class QCPItemTracer;
class HSGraphData;
class QCPGraph;
class QMouseEvent;
class ReceiveData;
class QCPAbstractPlottable;

namespace Ui {
class customGraph;
}

class customGraph : public QWidget
{
    Q_OBJECT

public:
    explicit customGraph(QWidget *parent = 0);
    ~customGraph();

    QCPAxis *xAxis;
    QCPAxis *yAxis;
    QCPAxis *yAxis2;
    QCPItemText *TextTip;
    QCPItemTracer *Trace;
    HSGraphData GraphData[LINE_NUM];

    void startBtn_enable(bool state);
    void clearBtn_enable(bool state);
    void saveBtn_enable(bool state);
    void resetBtn_enable(bool state);
    void openBtn_enable(bool state);
    void baseBtn_enable(bool state);
 //   void setTimer(int time);
    void updateInfo(ReceiveData *data);
//    void startRecord(bool state);
//    void autoSavedata(void); // 保存曲线数据

signals:

private slots:
    void graphClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);    //点击显示
    void onGraphHoverMove(QCPAbstractPlottable *plottable, QMouseEvent *event); //移动显示
    void graphMouseMoveEvent(QMouseEvent * event);  //解除坐标显示
    void ongraphMouseMove(QMouseEvent * event);
    void timer_out();
    void setTimer(int time);
    void startRecord(bool state);
    void autoSavedata(void); // 保存曲线数据
    void on_y1Max_editingFinished();
    void on_y1Min_editingFinished();
    void on_y2Max_editingFinished();
    void on_y2Min_editingFinished();
    void on_xMax_editingFinished();
    void on_xMin_editingFinished();
    void on_y1Check_toggled(bool checked);
    void on_y2Check_toggled(bool checked);
    void on_y3Check_toggled(bool checked);
    void on_y4Check_toggled(bool checked);
    void on_y5Check_toggled(bool checked);
    void on_y6Check_toggled(bool checked);
    void on_y7Check_toggled(bool checked);
    void on_y8Check_toggled(bool checked);
    void on_y9Check_toggled(bool checked);
    void on_y10Check_toggled(bool checked);
    void on_y11Check_toggled(bool checked);
    void on_y12Check_toggled(bool checked);
    void on_start_clicked();
    void on_clear_clicked();
    void on_save_clicked();
    void on_reset_clicked();
    void on_open_clicked();
    void on_export_graph_clicked();

    void open_graph_data(QString path);
    void save_graph_data(QString path);

private:
    Ui::customGraph *ui;
    QTimer timer;
    uint32_t sleepTime;
    void findClosestGraphPoint(const QPoint &mousePos, double mouseX);
    void showHoverInfo(QCPGraph *graph, double x, double y, const QPoint &mousePos);
};

#endif // CUSTOMGRAPH_H
