#include "customgraph.h"
#include <QScreen>

#include <QEvent>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QVector>
#include <QDateTime>
#include "customGraph/qcustomplot.h"
#include "customGraph/customTool/graphopen.h"
#include "customGraph/customTool/graphsave.h"
#include "customGraph/customTool/graphexport.h"
#include "strDecoder/portthread.h"
#include "common/mymessageBox.h"
#include "ui_customgraph.h"
#pragma execution_character_set("utf-8")

static QColor line_color[LINE_NUM] =
{
    QColor(255,0,0),      // 反应炉1 - 红
    QColor(0,255,0),      // 反应炉2 - 绿
    QColor(0,0,255),      // 温显1 - 蓝
    QColor(255,0,255),    // 温显2 - 紫
    QColor(0,255,255),    // 压力1-1 - 青
    QColor(255,128,0),    // 压力1-2 - 橙
    QColor(128,0,255),    // 压力1-3 - 紫蓝
    QColor(255,0,128),    // 柱塞泵1压力 - 粉红
    QColor(0,128,255),    // 压力2-1 - 天蓝
    QColor(128,255,0),    // 压力2-2 - 黄绿
    QColor(255,128,128),  // 压力2-3 - 浅红
    QColor(128,128,255),  // 柱塞泵2压力 - 浅蓝
};

static QString line_name[LINE_NUM] =
{
    "反应炉1",
    "反应炉2",
    "温显1",
    "温显2",
    "压力1-1",
    "压力1-2",
    "压力1-3",
    "柱塞泵1压力",
    "压力2-1",
    "压力2-2",
    "压力2-3",
    "柱塞泵2压力",
};

customGraph::customGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::customGraph)
{
    ui->setupUi(this);
    this->setWindowTitle("曲线查看");
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);

    sleepTime = 1000;  // 默认1s采集一个点

    ui->start->setToolTip("开始记录");
    ui->start->setIcon(QIcon(":/customGraph/image/start.png"));
    ui->start->setIconSize(QSize(30,30));

    ui->clear->setToolTip("清除曲线");
    ui->clear->setIcon(QIcon(":/customGraph/image/clear.png"));
    ui->clear->setIconSize(QSize(30,30));

    ui->save->setToolTip("保存数据");
    ui->save->setIcon(QIcon(":/customGraph/image/save.png"));
    ui->save->setIconSize(QSize(30,30));

    ui->reset->setToolTip("复位视图");
    ui->reset->setIcon(QIcon(":/customGraph/image/reset.png"));
    ui->reset->setIconSize(QSize(30,30));

    ui->open->setToolTip("打开曲线");
    ui->open->setIcon(QIcon(":/customGraph/image/open.png"));
    ui->open->setIconSize(QSize(30,30));

    ui->export_graph->setToolTip("导出曲线");
    ui->export_graph->setIcon(QIcon(":/customGraph/image/export.png"));
    ui->export_graph->setIconSize(QSize(30,30));
    ui->export_graph->hide();

//    ui->Graph->setOpenGl(true);
//    qDebug()<<"opengle="<<ui->Graph->openGl();

    xAxis = ui->Graph->xAxis; //x轴
    yAxis = ui->Graph->yAxis; //y轴  左边
    ui->Graph->addGraph(xAxis,yAxis);
    ui->Graph->addGraph(xAxis,yAxis);
    ui->Graph->addGraph(xAxis,yAxis);
    ui->Graph->addGraph(xAxis,yAxis);

    yAxis2 = ui->Graph->yAxis2; // y轴 右边
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->addGraph(xAxis,yAxis2);
    ui->Graph->yAxis2->setVisible(true);//显示y轴2

    //  设置坐标轴刻度
    ui->Graph->yAxis->setLabelColor(QColor(122, 79, 148));
    ui->Graph->xAxis->setLabelColor(QColor(122, 79, 148));
    ui->Graph->yAxis2->setLabelColor(QColor(122, 79, 148));
    QFont label_font;
    label_font.setFamily("SimHei");
    label_font.setPixelSize(18);
    ui->Graph->yAxis->setLabelFont(label_font); // 设置Y1轴名字字体
    ui->Graph->yAxis2->setLabelFont(label_font); // 设置Y2轴名字字体
    ui->Graph->xAxis->setLabelFont(label_font); // 设置X轴名字字体

    label_font.setPixelSize(15);
    ui->Graph->yAxis->setTickLabelFont(label_font); // y轴刻度数字字体
    ui->Graph->yAxis2->setTickLabelFont(label_font); // y2轴刻度数字字体
    ui->Graph->xAxis->setTickLabelFont(label_font); // x轴刻度数字字体

    ui->Graph->yAxis->setTickLabelColor(QColor(0, 0, 0));
    ui->Graph->yAxis2->setTickLabelColor(QColor(0, 0, 0));
    ui->Graph->xAxis->setBasePen(QPen(Qt::black,2));//设置下轴为黑色
    ui->Graph->yAxis->setBasePen(QPen(Qt::black,2));//设置左轴为黑色
    ui->Graph->yAxis2->setBasePen(QPen(Qt::black,2));//设置右轴为黑色
    ui->Graph->xAxis->ticker()->setTickCount(20);
    ui->Graph->yAxis->ticker()->setTickCount(16);
    ui->Graph->yAxis2->ticker()->setTickCount(16);
    //  设置网格
    ui->Graph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::SolidLine));     // 实线(对应刻度)画笔
    ui->Graph->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::SolidLine));   // 实线(对应刻度)画笔
    ui->Graph->yAxis2->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));    // 实线(对应刻度)画笔
    ui->Graph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine)); // 子网格线(对应子刻度)画笔
    ui->Graph->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->Graph->yAxis2->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->Graph->xAxis->grid()->setSubGridVisible(false);     // 不显示子网格线
    ui->Graph->yAxis->grid()->setSubGridVisible(false);
    ui->Graph->yAxis2->grid()->setSubGridVisible(false);

    ui->Graph->xAxis->setLabel("时间");  //设置轴的名字
    ui->Graph->xAxis->setTickLengthIn(1);       // 轴线内刻度的长度
    ui->Graph->xAxis->setTickLengthOut(5);      // 轴线外刻度的长度
 //   ui->Graph->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);  // 结束时加个箭头

    ui->Graph->yAxis->setLabel("温度(℃)");  //设置轴的名字
    ui->Graph->yAxis->setTickLengthIn(1);       // 轴线内刻度的长度
    ui->Graph->yAxis->setTickLengthOut(5);      // 轴线外刻度的长度

    ui->Graph->yAxis2->setLabel("压力(MPa))");  //设置轴的名字
    ui->Graph->yAxis2->setTickLengthIn(1);       // 轴线内刻度的长度
    ui->Graph->yAxis2->setTickLengthOut(5);      // 轴线外刻度的长度
//    ui->Graph->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);  // 结束时加个箭头

    //  设置坐标轴范围
    ui->Graph->xAxis->setRange(0,1000);
    ui->xMin->setText(QString::number(0));
    ui->xMax->setText(QString::number(1000));
    ui->Graph->yAxis->setRange(0,800);
    ui->y1Min->setText(QString::number(0));
    ui->y1Max->setText(QString::number(800));
    ui->Graph->yAxis2->setRange(0,5);
    ui->y2Min->setText(QString::number(0));
    ui->y2Max->setText(QString::number(5));

  //  ui->Graph->legend->setVisible(true);   // 显示曲线名称
    //  设置曲线显示样式
    QPen pen;
    pen.setWidth(2);//设置线宽
    pen.setStyle(Qt::PenStyle::SolidLine);//设置为实线
    for(int i=0;i<LINE_NUM;i++)
    {
       pen.setColor(line_color[i]);
       ui->Graph->graph(i)->setPen(pen);
       ui->Graph->graph(i)->setName(line_name[i]);  //  曲线名称
    }

    //  特殊功能设置
    QList<QCPAxis*> axes;
    axes << ui->Graph->yAxis2 << ui->Graph->xAxis2 << ui->Graph->yAxis << ui->Graph->xAxis;
    ui->Graph->axisRect()->setRangeZoomAxes(axes);
    ui->Graph->axisRect()->setRangeDragAxes(axes);

    ui->Graph->setSelectionRectMode(QCP::srmZoom);  //  矩形选中放大
    ui->Graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
   // ui->Graph->rescaleAxes(true);   //  坐标轴自适应

    // 设置区域外观
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
//    plotGradient.setColorAt(0, QColor(221, 221, 221));
//    plotGradient.setColorAt(1, QColor(221, 221, 221));
    plotGradient.setColorAt(0, Qt::transparent);
    plotGradient.setColorAt(1, Qt::transparent);   // 透明背景
    ui->Graph->setBackground(plotGradient);      // 设置背景颜色

    // 创建悬停指示器
    Trace = new QCPItemTracer(ui->Graph);
    Trace->setInterpolating(true);
    Trace->setStyle(QCPItemTracer::tsCrosshair);
    Trace->setPen(QPen(QColor(128, 128, 128), 1, Qt::DashLine));  // 灰色虚线，线宽1
    Trace->setBrush(Qt::NoBrush);
    Trace->setSize(8);
    Trace->setVisible(false);

    // 创建坐标标签
    TextTip = new QCPItemText(ui->Graph);
    TextTip->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    TextTip->position->setType(QCPItemPosition::ptAbsolute);
    QFont font;
    font.setPixelSize(15);
    TextTip->setFont(font);
    TextTip->setPen(QPen(Qt::transparent));
    TextTip->setBrush(Qt::white);
    TextTip->setVisible(false);
    //connect(ui->Graph, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)), this, SLOT(graphClick(QCPAbstractPlottable*, int, QMouseEvent*)));//点击显示坐标
    //connect(ui->Graph, SIGNAL(plottableHover(QCPAbstractPlottable*, int, QMouseEvent*)), this, SLOT(onGraphHoverMove(QCPAbstractPlottable*, QMouseEvent*)));//不支持悬停显示坐标
    //connect(ui->Graph,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(graphMouseMoveEvent(QMouseEvent*)));
    connect(ui->Graph,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(ongraphMouseMove(QMouseEvent*)));

    //  曲线的隐藏与显示  默认显示
    ui->y1Check->setChecked(true);
    ui->y1Check->setStyleSheet("QCheckBox{color:rgb(255,0,0)}");      // 反应炉1 - 红
    ui->y2Check->setChecked(true);
    ui->y2Check->setStyleSheet("QCheckBox{color:rgb(0,255,0)}");      // 反应炉2 - 绿
    ui->y3Check->setChecked(true);
    ui->y3Check->setStyleSheet("QCheckBox{color:rgb(0,0,255)}");      // 温显1 - 蓝
    ui->y4Check->setChecked(true);
    ui->y4Check->setStyleSheet("QCheckBox{color:rgb(255,0,255)}");    // 温显2 - 紫
    ui->y5Check->setChecked(true);
    ui->y5Check->setStyleSheet("QCheckBox{color:rgb(0,255,255)}");    // 压力1-1 - 青
    ui->y6Check->setChecked(true);
    ui->y6Check->setStyleSheet("QCheckBox{color:rgb(255,128,0)}");    // 压力1-2 - 橙
    ui->y7Check->setChecked(true);
    ui->y7Check->setStyleSheet("QCheckBox{color:rgb(128,0,255)}");    // 压力1-3 - 紫蓝
    ui->y8Check->setChecked(true);
    ui->y8Check->setStyleSheet("QCheckBox{color:rgb(255,0,128)}");    // 柱塞泵1压力 - 粉红
    ui->y9Check->setChecked(true);
    ui->y9Check->setStyleSheet("QCheckBox{color:rgb(0,128,255)}");    // 压力2-1 - 天蓝
    ui->y10Check->setChecked(true);
    ui->y10Check->setStyleSheet("QCheckBox{color:rgb(128,255,0)}");   // 压力2-2 - 黄绿
    ui->y11Check->setChecked(true);
    ui->y11Check->setStyleSheet("QCheckBox{color:rgb(255,128,128)}"); // 压力2-3 - 浅红
    ui->y12Check->setChecked(true);
    ui->y12Check->setStyleSheet("QCheckBox{color:rgb(128,128,255)}"); // 柱塞泵2压力 - 浅蓝

    connect(&timer,SIGNAL(timeout()),this,SLOT(timer_out()));
}

customGraph::~customGraph()
{
    delete ui;
}

void customGraph::startBtn_enable(bool state)
{
     ui->start->setDisabled(!state);
}

void customGraph::clearBtn_enable(bool state)
{
     ui->clear->setDisabled(!state);
}

void customGraph::saveBtn_enable(bool state)
{
     ui->save->setDisabled(!state);
}

void customGraph::resetBtn_enable(bool state)
{
    ui->reset->setDisabled(!state);
}

void customGraph::openBtn_enable(bool state)
{
    ui->open->setDisabled(!state);
}

void customGraph::setTimer(int time)
{
     sleepTime = time;
     if(timer.isActive())  // 如果已经处于计时
     {
        timer.start(sleepTime);
     }
}

void customGraph::updateInfo(ReceiveData *data)
{
    GraphData[0].temp = data->PointTemp[0];  // 反应炉1
    GraphData[1].temp = data->PointTemp[1];  // 反应炉2
    GraphData[2].temp = data->PointTemp[2];  // 温显1
    GraphData[3].temp = data->PointTemp[3];  // 温显2
    GraphData[4].temp = data->pres[0];  // 压力1
    GraphData[5].temp = data->pres[1];  // 压力2
    GraphData[6].temp = data->pres[6];  // 压力3
    GraphData[7].temp = data->pres[4];  // 泵压力
    GraphData[8].temp = data->pres[2];  // 压力1
    GraphData[9].temp = data->pres[3];  // 压力2
    GraphData[10].temp = data->pres[7];  // 压力3
    GraphData[11].temp = data->pres[5];  // 泵压力
}

void customGraph::startRecord(bool state)
{
    if(state)  // 开始记录
    {
       if(timer.isActive())  // 已经在记录了
       {
           return ;
       }
       else
       {
           ui->start->setToolTip("停止记录");
           ui->start->setIcon(QIcon(":/customGraph/image/stop.png"));
           timer.start(sleepTime);
       }
    }
    else
    {
        if(!timer.isActive())   //  已经停止记录了
        {
           return ;
        }
        else
        {
            ui->start->setToolTip("开始记录");
            ui->start->setIcon(QIcon(":/customGraph/image/start.png"));
            timer.stop();
        }
    }
}

void customGraph::autoSavedata()
{
    QString path = QCoreApplication::applicationDirPath()+"/data";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    // 获取当前时间作为文件名
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date ="自动保存"+current_date_time.toString("yyyyMMddhhmmss");
    QString str = path+"/"+current_date+".csv";  // 得到文件名
    QFile file(str);
    file.open( QIODevice::ReadWrite | QIODevice::Text );
    QTextStream out(&file);
    QString title = "序号,";
    int i,j;
    for(i=0;i<LINE_NUM;i++)
    {
       title = title+line_name[i]+",";
    }
    title = title + "\n";
    out<<tr(title.toStdString().c_str());

    QString content;
    for(i=0;i<GraphData[0].x;i++)
    {
       content = QString::number(i+1)+",";
       for(j=0;j<LINE_NUM;j++)
       {
         content = content + QString::number(GraphData[j].y_val.at(i))+",";
       }
       content = content + "\n";
       out<<tr(content.toStdString().c_str());
       content.clear();
    }
    file.close();
}

void customGraph::graphClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
   // 通过曲线名称选择id
   int id = 0;
   if(plottable->name() == line_name[0])
   {
      id = 0;
   }
   else if(plottable->name() == line_name[1])
   {
      id = 1;
   }
   const QCPGraphData *ghd = ui->Graph->graph(id)->data()->at(dataIndex);
   QString text = "(" + QString::number(ghd->key,10,0) + "," + QString::number(ghd->value,10,1) + ")";
   TextTip->setText(text);//文本内容填充
   TextTip->position->setCoords(event->pos().x()+30, event->pos().y()-15);//文本框所在位置
   TextTip->setVisible(true);
   ui->Graph->replot();
}

void customGraph::onGraphHoverMove(QCPAbstractPlottable *plottable, QMouseEvent *event)
{
    QCPGraph *graph = qobject_cast<QCPGraph*>(plottable);
    if (!graph) return;

    // 获取鼠标位置对应的数据坐标
    double x = ui->Graph->xAxis->pixelToCoord(event->pos().x());
    double y = ui->Graph->yAxis->pixelToCoord(event->pos().y());

    // 查找最近的数据点
    QCPGraphDataContainer::const_iterator closestIt = graph->data()->constEnd();
    double minDistSqr = std::numeric_limits<double>::max();

    for (auto it = graph->data()->constBegin(); it != graph->data()->constEnd(); ++it)
    {
        double distSqr = (x - it->key) * (x - it->key) + (y - it->value) * (y - it->value);
        if (distSqr < minDistSqr) {
            minDistSqr = distSqr;
            closestIt = it;
        }
    }

    if (closestIt != graph->data()->constEnd() && minDistSqr < 0.01)
    {
        // 显示跟踪点和标签
        Trace->setGraphKey(closestIt->key);
        Trace->updatePosition();
        Trace->setVisible(true);

        // 更新标签位置为鼠标位置
        TextTip->position->setPixelPosition(event->pos() + QPoint(10, -10));
        TextTip->setText(QString("X: %1\nY: %2")
                         .arg(closestIt->key, 0, 'f', 1)
                         .arg(closestIt->value, 0, 'f', 1));
        TextTip->setVisible(true);
    }
    else
    {
        Trace->setVisible(false);
        TextTip->setVisible(false);
    }

    ui->Graph->replot();
}

void customGraph::graphMouseMoveEvent(QMouseEvent *event)
{
    (void)event;
    TextTip->setVisible(false);
}

void customGraph::ongraphMouseMove(QMouseEvent *event)
{
    Trace->setVisible(false);
    TextTip->setVisible(false);
    // 获取鼠标位置对应的坐标
    double mouseX = ui->Graph->xAxis->pixelToCoord(event->pos().x());
    //double mouseY = ui->Graph->yAxis->pixelToCoord(event->pos().y());

    // 查找最近的曲线和数据点
    findClosestGraphPoint(event->pos(), mouseX);

    ui->Graph->replot();
}

void customGraph::findClosestGraphPoint(const QPoint &mousePos, double mouseX)
{
    QCPGraph *closestGraph = nullptr;
    double closestKey = 0, closestValue = 0;
    double minDistance = 20.0; // 像素容差

    // 遍历所有曲线
    for (int i = 0; i < ui->Graph->graphCount(); ++i) {
        QCPGraph *graph = ui->Graph->graph(i);
        if (!graph || !graph->visible()) continue;

        // 查找最近的X值
        auto data = graph->data();
        auto it = data->findBegin(mouseX);

        if (it != data->constEnd()) {
            // 转换为像素坐标
            QPointF pointPos = graph->coordsToPixels(it->key, it->value);
            double distance = QLineF(mousePos, pointPos).length();

            if (distance < minDistance) {
                minDistance = distance;
                closestGraph = graph;
                closestKey = it->key;
                closestValue = it->value;
            }
        }
    }

    // 显示悬停信息
    if (closestGraph) {
        showHoverInfo(closestGraph, closestKey, closestValue, mousePos);
    }
}

void customGraph::showHoverInfo(QCPGraph *graph, double x, double y, const QPoint &mousePos)
{
    // 关联跟踪器到当前曲线
    Trace->setGraph(graph);
    Trace->setGraphKey(x);
    Trace->updatePosition();
    Trace->setVisible(true);

    // 更新标签
    QString curveName = graph->name().isEmpty() ? "曲线" : graph->name();
    QString text = QString("%1\nX: %2\nY: %3")
                   .arg(curveName)
                   .arg(x, 0, 'f', 1)
                   .arg(y, 0, 'f', 1);

    TextTip->setText(text);
    TextTip->position->setPixelPosition(mousePos + QPoint(45, -60));
    TextTip->setVisible(true);
}

void customGraph::timer_out()
{
    int i;
    for(i=0;i<LINE_NUM;i++)
    {
       GraphData[i].x_val.append(GraphData[i].x);
       GraphData[i].y_val.append(GraphData[i].temp);
       GraphData[i].x++;
       ui->Graph->graph(i)->setData(GraphData[i].x_val,GraphData[i].y_val);
    }
    ui->Graph->replot();
}

void customGraph::on_y1Max_editingFinished()
{
    ui->Graph->yAxis->setRange(ui->y1Min->text().toInt(),ui->y1Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y1Min_editingFinished()
{
    ui->Graph->yAxis->setRange(ui->y1Min->text().toInt(),ui->y1Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y2Max_editingFinished()
{
    ui->Graph->yAxis2->setRange(ui->y2Min->text().toInt(),ui->y2Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y2Min_editingFinished()
{
    ui->Graph->yAxis2->setRange(ui->y2Min->text().toInt(),ui->y2Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void customGraph::on_xMax_editingFinished()
{
    ui->Graph->xAxis->setRange(ui->xMin->text().toInt(),ui->xMax->text().toInt());
    ui->Graph->replot();//重新绘制
}

void customGraph::on_xMin_editingFinished()
{
    ui->Graph->xAxis->setRange(ui->xMin->text().toInt(),ui->xMax->text().toInt());
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y1Check_toggled(bool checked)
{
    ui->Graph->graph(0)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y2Check_toggled(bool checked)
{
    ui->Graph->graph(1)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y3Check_toggled(bool checked)
{
    ui->Graph->graph(2)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y4Check_toggled(bool checked)
{
    ui->Graph->graph(3)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y5Check_toggled(bool checked)
{
    ui->Graph->graph(4)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y6Check_toggled(bool checked)
{
    ui->Graph->graph(5)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y7Check_toggled(bool checked)
{
    ui->Graph->graph(6)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y8Check_toggled(bool checked)
{
    ui->Graph->graph(7)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y9Check_toggled(bool checked)
{
    ui->Graph->graph(8)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y10Check_toggled(bool checked)
{
    ui->Graph->graph(9)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y11Check_toggled(bool checked)
{
    ui->Graph->graph(10)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_y12Check_toggled(bool checked)
{
    ui->Graph->graph(11)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void customGraph::on_start_clicked()
{
    if(ui->start->toolTip() == "开始记录")
    {
       ui->start->setToolTip("停止记录");
       ui->start->setIcon(QIcon(":/customGraph/image/stop.png"));
       timer.start(sleepTime);
    }
    else
    {
       ui->start->setToolTip("开始记录");
       ui->start->setIcon(QIcon(":/customGraph/image/start.png"));
       timer.stop();
    }
}

void customGraph::on_clear_clicked()
{
    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"清除曲线?","提示",MyMessageBox::Yes | MyMessageBox::No);
    switch (result)
    {
        case MyMessageBox::Yes:

             break;
        case MyMessageBox::No:
             return ;

             break;
        default:
             break;
    }

    int i;
    for(i=0;i<LINE_NUM;i++)
    {
        ui->Graph->graph(i)->data().data()->clear();
        GraphData[i].clear();
    }
    ui->Graph->replot();//重新绘制
}

void customGraph::on_save_clicked()
{
#ifndef Q_OS_WIN

    graphSave *file_save = new graphSave;
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    int x = (screen.width() - file_save->width()) / 2;
    int y = (screen.height() - file_save->height()) / 2;
    file_save->move(x,y);

    connect(file_save,&graphSave::file_saved,this,&customGraph::save_graph_data);

    file_save->raise();
    file_save->show();

#else

    QString path = QCoreApplication::applicationDirPath()+"/data";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    QString str = QFileDialog::getSaveFileName(this,"save",path,"(*.csv)");
    if(!str.isEmpty())
    {
        QFile file(str);
        file.open( QIODevice::WriteOnly  | QIODevice::Text | QIODevice::Truncate);
        QTextStream out(&file);
        QString title = "序号,";
        int i,j;
        for(i=0;i<LINE_NUM;i++)
        {
           title = title+line_name[i]+",";
        }
        title = title + "\n";
        out<<tr(title.toStdString().c_str());

        QString content;
        for(i=0;i<GraphData[0].x;i++)
        {
           content = QString::number(i+1)+",";
           for(j=0;j<LINE_NUM;j++)
           {
             content = content + QString::number(GraphData[j].y_val.at(i))+",";
           }
           content = content + "\n";
           out<<tr(content.toStdString().c_str());
           content.clear();
        }
        file.close();
    }

#endif
}

void customGraph::on_reset_clicked()
{
    ui->Graph->xAxis->setRange(ui->xMin->text().toInt(),ui->xMax->text().toInt());
    ui->Graph->yAxis->setRange(ui->y1Min->text().toInt(),ui->y1Max->text().toInt());
    ui->Graph->yAxis2->setRange(ui->y2Min->text().toInt(),ui->y2Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void customGraph::on_open_clicked()
{
#ifndef Q_OS_WIN

    graphOpen *file_open = new graphOpen;
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    int x = (screen.width() - file_open->width()) / 2;
    int y = (screen.height() - file_open->height()) / 2;
    file_open->move(x,y);

    connect(file_open,&graphOpen::file_to_open,this,&customGraph::open_graph_data);

    file_open->raise();
    file_open->show();

#else

    QString path = QCoreApplication::applicationDirPath()+"/data";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    int i;
    if(timer.isActive())   //  判断是否正在记录中
    {
        customGraph *graph = new customGraph;     //  重新开一个界面显示曲线
        graph->startBtn_enable(false);   //  禁用不许要用到的按钮
        graph->saveBtn_enable(false);
        graph->clearBtn_enable(false);
        graph->show();
        graph->on_open_clicked();
        return;
    }
    QString str = QFileDialog::getOpenFileName(this,"open",path,"(*.csv)");  // 打开文件
    if(str == "")
    {
       return;    //  “取消” 退出
    }
    QList<QStringList> readData;
    QFile readFile(str);
    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;

    QTextStream stream(&readFile);    //  读取文件
    QString line;
    QStringList row;
    int lineSize = 0; //  数据行数  含数据项名称
    while (!stream.atEnd())
    {
        line = stream.readLine();
        row = line.split(',', QString::SkipEmptyParts);
        readData.append(row);
        lineSize++;
    }
    readFile.close();

    if(lineSize == 0)
    {
         return ;
    }
    for(i=0;i<LINE_NUM;i++)    // 判断文件头与曲线是否匹配
    {
       if(readData.at(0).at(i+1) != line_name[i])  //  (i+1)是跳过开头的“序号”项
       {
          MyMessageBox msg(MyMessageBox::Critical, "提示", "格式不匹配！", MyMessageBox::Ok,this);
          msg.exec();
          return;
       }
    }

    on_clear_clicked();  //先清除曲线

    QString name;
    int cnt;
    for(cnt = 1;cnt < lineSize;cnt++) // 跳过第一行，然后处理每一行
    {
        for(i=0;i<LINE_NUM;i++)
        {
            name = readData.at(cnt).at(i+1);
            GraphData[i].temp = name.toDouble();
            GraphData[i].x_val.append(GraphData[i].x);
            GraphData[i].y_val.append(GraphData[i].temp);
            GraphData[i].x++;
        }
    }
    for(i=0;i<LINE_NUM;i++)
    {
       ui->Graph->graph(i)->setData(GraphData[i].x_val,GraphData[i].y_val);
    }
    ui->Graph->replot();

#endif
}

void customGraph::on_export_graph_clicked()
{
    graphExport *file_export = new graphExport;
    QScreen* desktop = QApplication::primaryScreen();
    QRect screen = desktop->geometry();
    int x = (screen.width() - file_export->width()) / 2;
    int y = (screen.height() - file_export->height()) / 2;
    file_export->move(x,y);

    file_export->raise();
    file_export->show();
}

void customGraph::open_graph_data(QString path)
{
    if(timer.isActive())  // 正在记录中
    {
        MyMessageBox msg(MyMessageBox::Critical, "提示", "请先停止记录！", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }

    int i;
    QList<QStringList> readData;
    QFile readFile(path);
    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;

    QTextStream stream(&readFile);    //  读取文件
    QString line;
    QStringList row;
    int lineSize = 0; //  数据行数  含数据项名称
    while (!stream.atEnd())
    {
        line = stream.readLine();
        row = line.split(',',QString::SkipEmptyParts);
        readData.append(row);
        lineSize++;
    }
    readFile.close();

    if(lineSize == 0)
    {
         return ;
    }

    for(i=0;i<LINE_NUM;i++)    // 判断文件头与曲线是否匹配
    {
       if(readData.at(0).at(i+1) != line_name[i])  //  (i+1)是跳过开头的“序号”项
       {
          MyMessageBox msg(MyMessageBox::Critical, "提示", "格式不匹配！", MyMessageBox::Ok,this);
          msg.exec();
          return;
       }
    }

    //先清除曲线
    for(i=0;i<LINE_NUM;i++)
    {
        ui->Graph->graph(i)->data().data()->clear();
        GraphData[i].clear();
    }
    ui->Graph->replot();//重新绘制

    QString name;
    int cnt;
    for(cnt = 1;cnt < lineSize;cnt++) // 跳过第一行，然后处理每一行
    {
        for(i=0;i<LINE_NUM;i++)
        {
            name = readData.at(cnt).at(i+1);
            GraphData[i].temp = name.toDouble();
            GraphData[i].x_val.append(GraphData[i].x);
            GraphData[i].y_val.append(GraphData[i].temp);
            GraphData[i].x++;
        }
    }
    for(i=0;i<LINE_NUM;i++)
    {
       ui->Graph->graph(i)->setData(GraphData[i].x_val,GraphData[i].y_val);
    }
    ui->Graph->replot();
}

void customGraph::save_graph_data(QString path)
{
    if(!path.isEmpty())
    {
        QFile file(path);

        if(file.exists()) //如果文件存在
        {
            MyMessageBox:: StandardButton result = MyMessageBox::information(this,"覆盖已有文件?","提示",MyMessageBox::Yes | MyMessageBox::No);
            switch (result)
            {
                case MyMessageBox::Yes:

                     break;
                case MyMessageBox::No:
                     return ;  // 退出
                     break;
                default:
                     break;
            }
        }

        file.open( QIODevice::WriteOnly  | QIODevice::Text | QIODevice::Truncate);
        QTextStream out(&file);
        QString title = "序号,";
        int i,j;
        for(i=0;i<LINE_NUM;i++)
        {
           title = title+line_name[i]+",";
        }
        title = title + "\n";
        out<<tr(title.toStdString().c_str());

        QString content;
        for(i=0;i<GraphData[0].x;i++)
        {
           content = QString::number(i+1)+",";
           for(j=0;j<LINE_NUM;j++)
           {
             content = content + QString::number(GraphData[j].y_val.at(i))+",";
           }
           content = content + "\n";
           out<<tr(content.toStdString().c_str());
           content.clear();
        }
        file.close();
    }
}
