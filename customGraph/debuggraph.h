#ifndef DEBUGGRAPH_H
#define DEBUGGRAPH_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QVector>
#include <QTimer>
#include <QDebug>
#include "customGraph/qcustomplot.h"
#include "strDecoder/portthread.h"
#include "common/mymessageBox.h"

#define DEBUG_LINE_NUM 3

class DebugGraphData
{
public:
       int x;
       double temp;
       QVector<double> x_val, y_val;

       DebugGraphData()
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

namespace Ui {
class debugGraph;
}

class debugGraph : public QWidget
{
    Q_OBJECT

public:
    explicit debugGraph(QWidget *parent = 0);
    ~debugGraph();

    QCPAxis *xAxis;
    QCPAxis *yAxis;
    QCPAxis *yAxis2;
    QCPItemText *TextTip;
    DebugGraphData GraphData[DEBUG_LINE_NUM];

    void startBtn_enable(bool state);
    void clearBtn_enable(bool state);
    void saveBtn_enable(bool state);
    void resetBtn_enable(bool state);
    void openBtn_enable(bool state);
    void baseBtn_enable(bool state);
    void setTimer(uint32_t time);
    void updateInfo(ReceiveData *data);
signals:
    void baseAdjust(void);
private slots:
    void graphClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void graphMouseMoveEvent(QMouseEvent * event);
    void timer_out();
    void on_y1Max_editingFinished();
    void on_y1Min_editingFinished();
    void on_y2Max_editingFinished();
    void on_y2Min_editingFinished();
    void on_xMax_editingFinished();
    void on_xMin_editingFinished();
    void on_y1Check_toggled(bool checked);
    void on_y2Check_toggled(bool checked);
    void on_y3Check_toggled(bool checked);
    void on_start_clicked();
    void on_clear_clicked();
    void on_save_clicked();
    void on_reset_clicked();
    void on_open_clicked();
    void on_baseLine_clicked();


private:
    Ui::debugGraph *ui;
    QTimer timer;
    uint32_t sleepTime;
};

#endif // DEBUGGRAPH_H
