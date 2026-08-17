#include "debuggraph.h"
#include "ui_debuggraph.h"
#pragma execution_character_set("utf-8")

static QColor line_color[DEBUG_LINE_NUM] =
{
    QColor(255,0,0),
    QColor(0,255,0),
    QColor(0,0,255),
};

static QString line_name[DEBUG_LINE_NUM] =
{
    "反应炉温度",
    "TCD温度",
    "信号",
};

debugGraph::debugGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::debugGraph)
{
    ui->setupUi(this);
    this->setWindowTitle("曲线查看");
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose, false);

    sleepTime = 1000;  // 默认1s采集一个点

    ui->start->setToolTip("开始记录");
    ui->start->setIcon(QIcon(":/customGraph/image/start.png"));
    ui->clear->setToolTip("清除曲线");
    ui->clear->setIcon(QIcon(":/customGraph/image/clear.png"));
    ui->save->setToolTip("保存数据");
    ui->save->setIcon(QIcon(":/customGraph/image/save.png"));
    ui->reset->setToolTip("复位视图");
    ui->reset->setIcon(QIcon(":/customGraph/image/reset.png"));
    ui->open->setToolTip("打开曲线");
    ui->open->setIcon(QIcon(":/customGraph/image/open.png"));
    ui->baseLine->setToolTip("基准调零");
    ui->baseLine->setIcon(QIcon(":/customGraph/image/baseline.png"));

    xAxis = ui->Graph->xAxis;//x轴
    yAxis = ui->Graph->yAxis;//y轴  左边
    ui->Graph->addGraph(xAxis,yAxis);// 增加曲线  反应炉温度
    ui->Graph->addGraph(xAxis,yAxis);// 增加曲线  TCD温度

    yAxis2 = ui->Graph->yAxis2; // y轴 右边
    ui->Graph->addGraph(xAxis,yAxis2);// 增加曲线  信号
    ui->Graph->yAxis2->setVisible(true);//显示y轴2

    //  设置坐标轴刻度
    ui->Graph->xAxis->setLabelColor(QColor(0, 0, 0));
    ui->Graph->xAxis->setTickLabelColor(QColor(0, 0, 0));
    ui->Graph->yAxis->setLabelColor(QColor(255, 0, 0));
    ui->Graph->yAxis->setTickLabelColor(QColor(255, 0, 0));
    ui->Graph->yAxis2->setLabelColor(QColor(0, 0, 255));
    ui->Graph->yAxis2->setTickLabelColor(QColor(0, 0, 255));
    ui->Graph->xAxis->setBasePen(QPen(Qt::black,2));//设置下轴为黑色
    ui->Graph->yAxis->setBasePen(QPen(Qt::red,2));//设置左轴为红色
    ui->Graph->yAxis2->setBasePen(QPen(Qt::blue,2));//设置右轴为蓝色
    ui->Graph->xAxis->ticker()->setTickCount(10);
    ui->Graph->yAxis->ticker()->setTickCount(8);
    ui->Graph->yAxis2->ticker()->setTickCount(8);
    //  设置网格
    ui->Graph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));     // 网格线(对应刻度)画笔
    ui->Graph->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->Graph->yAxis2->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->Graph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine)); // 子网格线(对应子刻度)画笔
    ui->Graph->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->Graph->yAxis2->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->Graph->xAxis->grid()->setSubGridVisible(true);     // 显示子网格线
    ui->Graph->yAxis->grid()->setSubGridVisible(true);
    ui->Graph->yAxis2->grid()->setSubGridVisible(true);

    ui->Graph->xAxis->setLabel("时间");  //设置轴的名字
    ui->Graph->xAxis->setTickLengthIn(1);       // 轴线内刻度的长度
    ui->Graph->xAxis->setTickLengthOut(5);      // 轴线外刻度的长度
 //   ui->Graph->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);  // 结束时加个箭头

    ui->Graph->yAxis->setLabel("温度(℃)");  //设置轴的名字
    ui->Graph->yAxis->setTickLengthIn(1);       // 轴线内刻度的长度
    ui->Graph->yAxis->setTickLengthOut(5);      // 轴线外刻度的长度

    ui->Graph->yAxis2->setLabel("信号(mV))");  //设置轴的名字
    ui->Graph->yAxis2->setTickLengthIn(1);       // 轴线内刻度的长度
    ui->Graph->yAxis2->setTickLengthOut(5);      // 轴线外刻度的长度
 //   ui->Graph->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);  // 结束时加个箭头

    //  设置坐标轴范围
    ui->Graph->xAxis->setRange(0,1000);
    ui->xMin->setText(QString::number(0));
    ui->xMax->setText(QString::number(1000));
    ui->Graph->yAxis->setRange(0,800);
    ui->y1Min->setText(QString::number(0));
    ui->y1Max->setText(QString::number(800));
    ui->Graph->yAxis2->setRange(-40,40);
    ui->y2Min->setText(QString::number(-40));
    ui->y2Max->setText(QString::number(40));

 //   ui->Graph->legend->setVisible(true);   // 显示曲线名称
    //  设置曲线显示样式
    QPen pen;
    pen.setWidth(2);//设置线宽
    pen.setStyle(Qt::PenStyle::SolidLine);//设置为实线
    for(int i=0;i<DEBUG_LINE_NUM;i++)
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
  //  ui->Graph->rescaleAxes(true);   //  坐标轴自适应
    ui->Graph->replot();

    // 坐标浮窗
    TextTip = new QCPItemText(ui->Graph);
    TextTip->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    TextTip->position->setType(QCPItemPosition::ptAbsolute);
    QFont font;
    font.setPixelSize(15);
    TextTip->setFont(font);
    TextTip->setPen(QPen(Qt::transparent)); // 边框透明
    TextTip->setBrush(Qt::white);
    TextTip->setVisible(false);
    connect(ui->Graph, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)), this, SLOT(graphClick(QCPAbstractPlottable*, int, QMouseEvent*)));//点击显示坐标
    connect(ui->Graph,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(graphMouseMoveEvent(QMouseEvent*)));
    //  曲线的隐藏与显示  默认显示
    ui->y1Check->setChecked(true);
    ui->y1Check->setStyleSheet("QCheckBox{color:rgb(255,0,0)}");
    ui->y2Check->setChecked(true);
    ui->y2Check->setStyleSheet("QCheckBox{color:rgb(0,255,0)}");
    ui->y3Check->setChecked(true);
    ui->y3Check->setStyleSheet("QCheckBox{color:rgb(0,0,255)}");

    connect(&timer,SIGNAL(timeout()),this,SLOT(timer_out()));
}

debugGraph::~debugGraph()
{
    delete ui;
}

void debugGraph::startBtn_enable(bool state)
{
     ui->start->setDisabled(!state);
}

void debugGraph::clearBtn_enable(bool state)
{
     ui->clear->setDisabled(!state);
}

void debugGraph::saveBtn_enable(bool state)
{
     ui->save->setDisabled(!state);
}

void debugGraph::resetBtn_enable(bool state)
{
    ui->reset->setDisabled(!state);
}

void debugGraph::openBtn_enable(bool state)
{
    ui->open->setDisabled(!state);
}

void debugGraph::baseBtn_enable(bool state)
{
    ui->baseLine->setDisabled(!state);
}

void debugGraph::setTimer(uint32_t time)
{
     sleepTime = time;
     if(timer.isActive())  // 如果已经处于计时
     {
        timer.start(sleepTime);
     }
}

void debugGraph::updateInfo(ReceiveData *data)
{
    GraphData[0].temp = data->PointTemp[0];  // 反应炉温度
    GraphData[1].temp = data->PointTemp[1];  // TCD温度
//    GraphData[2].temp = data->tcd_signal;  // TCD信号
}

void debugGraph::graphClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
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
    else if(plottable->name() == line_name[2])
    {
       id = 2;
    }
    const QCPGraphData *ghd = ui->Graph->graph(id)->data()->at(dataIndex);
    QString text = "(" + QString::number(ghd->key,10,0) + "," + QString::number(ghd->value,10,1) + ")";
    TextTip->setText(text);//文本内容填充
    TextTip->position->setCoords(event->pos().x()+30, event->pos().y()-15);//文本框所在位置
    TextTip->setVisible(true);
    ui->Graph->replot();
}

void debugGraph::graphMouseMoveEvent(QMouseEvent *event)
{
    (void)event;
    TextTip->setVisible(false);
}

void debugGraph::timer_out()
{
    int i;
    for(i=0;i<DEBUG_LINE_NUM;i++)
    {
       GraphData[i].x_val.append(GraphData[i].x);
       GraphData[i].y_val.append(GraphData[i].temp);
       GraphData[i].x++;
       ui->Graph->graph(i)->setData(GraphData[i].x_val,GraphData[i].y_val);
    }
    ui->Graph->replot();
}

void debugGraph::on_y1Max_editingFinished()
{
    ui->Graph->yAxis->setRange(ui->y1Min->text().toInt(),ui->y1Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_y1Min_editingFinished()
{
    ui->Graph->yAxis->setRange(ui->y1Min->text().toInt(),ui->y1Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_y2Max_editingFinished()
{
    ui->Graph->yAxis2->setRange(ui->y2Min->text().toInt(),ui->y2Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_y2Min_editingFinished()
{
    ui->Graph->yAxis2->setRange(ui->y2Min->text().toInt(),ui->y2Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_xMax_editingFinished()
{
    ui->Graph->xAxis->setRange(ui->xMin->text().toInt(),ui->xMax->text().toInt());
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_xMin_editingFinished()
{
    ui->Graph->xAxis->setRange(ui->xMin->text().toInt(),ui->xMax->text().toInt());
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_y1Check_toggled(bool checked)
{
    ui->Graph->graph(0)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_y2Check_toggled(bool checked)
{
    ui->Graph->graph(1)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_y3Check_toggled(bool checked)
{
    ui->Graph->graph(2)->setVisible(checked);
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_start_clicked()
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

void debugGraph::on_clear_clicked()
{
    int i;
    for(i=0;i<DEBUG_LINE_NUM;i++)
    {
        ui->Graph->graph(i)->data().data()->clear();
        GraphData[i].clear();
    }
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_save_clicked()
{
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
        for(i=0;i<DEBUG_LINE_NUM;i++)
        {
           title = title+line_name[i]+",";
        }
        title = title + "\n";
        out<<tr(title.toStdString().c_str());

        QString content;
        for(i=0;i<GraphData[0].x;i++)
        {
           content = QString::number(i+1)+",";
           for(j=0;j<DEBUG_LINE_NUM;j++)
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

void debugGraph::on_reset_clicked()
{
    ui->Graph->xAxis->setRange(ui->xMin->text().toInt(),ui->xMax->text().toInt());
    ui->Graph->yAxis->setRange(ui->y1Min->text().toInt(),ui->y1Max->text().toInt());
    ui->Graph->yAxis2->setRange(ui->y2Min->text().toInt(),ui->y2Max->text().toInt());
    ui->Graph->replot();//重新绘制
}

void debugGraph::on_open_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/data";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    int i;
    if(timer.isActive())   //  判断是否正在记录中
    {
        debugGraph *graph = new debugGraph;     //  重新开一个界面显示曲线
        graph->startBtn_enable(false);   //  禁用不许要用到的按钮
        graph->saveBtn_enable(false);
        graph->clearBtn_enable(false);
        graph->baseBtn_enable(false);
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
        row = line.split(',',QString::SkipEmptyParts);
        readData.append(row);
        lineSize++;
    }
    readFile.close();

    if(lineSize == 0)
    {
         return ;
    }
    for(i=0;i<DEBUG_LINE_NUM;i++)    // 判断文件头与曲线是否匹配
    {
       if(readData.at(0).at(i+1) != line_name[i])  //  (i+1)是跳过开头的“序号”项
       {
           MyMessageBox msg(MyMessageBox::Critical, "提示", "格式不匹配！", MyMessageBox::Ok);
           msg.exec();
          return;
       }
    }

    on_clear_clicked();  //先清除曲线

    QString name;
    int cnt;
    for(cnt = 1;cnt < lineSize;cnt++) // 跳过第一行，然后处理每一行
    {
        for(i=0;i<DEBUG_LINE_NUM;i++)
        {
            name = readData.at(cnt).at(i+1);
            GraphData[i].temp = name.toDouble();
            GraphData[i].x_val.append(GraphData[i].x);
            GraphData[i].y_val.append(GraphData[i].temp);
            GraphData[i].x++;
        }
    }
    for(i=0;i<DEBUG_LINE_NUM;i++)
    {
       ui->Graph->graph(i)->setData(GraphData[i].x_val,GraphData[i].y_val);
    }
    ui->Graph->replot();
}

void debugGraph::on_baseLine_clicked()
{
     emit baseAdjust();
}
