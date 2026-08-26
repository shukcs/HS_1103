#include "programlist.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGridLayout>
#include <QScrollBar>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QList>
#include <QCoreApplication>
#include <QScrollArea>
#include "common/mymessageBox.h"
#include "customTool/ToolBox.h"
#include "customTool/objlist.h"
#include "materialFeeder/FeederMgr.h"
#pragma execution_character_set("utf-8")

ProgramList::ProgramList(QWidget *parent)
    : QWidget(parent)
{
    state = false;
    runTime = 0;
    programCnt = -1;
    totalCnt = 0;
    circulationCnt = 0;
    totalcirculationCnt = 0;
    delayTime = 0;
    CirculateStartLine = 0;
    CirculateStopLine = 0;

    m_titles.clear(); // 清除所有数据

    QHBoxLayout *main_layout = new QHBoxLayout(this);
    main_layout->setMargin(2);
    main_layout->setSpacing(2);
    QWidget *w1 = new QWidget;
    w1->setMaximumWidth(250);
    w1->setObjectName("w1");
    QWidget *w2 = new QWidget;
    w2->setObjectName("w2");
    w1->setStyleSheet("QWidget#w1{background-color: rgb(220, 220, 220);border-radius: 15px;border: 1px solid rgb(239, 239, 239);}");
    w2->setStyleSheet("QWidget#w2{background-color: rgb(220, 220, 220);border-radius: 15px;border: 1px solid rgb(239, 239, 239);}");

    main_layout->addWidget(w1);
    main_layout->addWidget(w2);

    QVBoxLayout *Vlayout = new QVBoxLayout(w1);//  左边垂直布局
    Vlayout->setMargin(0);

//    QPushButton *btntest = new QPushButton;
//    btntest->setFixedSize(200,30);
//    btntest->setText("hello");
//    V2layout->addWidget(btntest);

    QWidget *topWidget = new QWidget();
    topWidget->setFixedHeight(36);
    Vlayout->addWidget(topWidget);

    QHBoxLayout *Toplayout = new QHBoxLayout;
    Toplayout->setMargin(0);
    topWidget->setLayout(Toplayout);

    Toplayout->addStretch();

    run = new QPushButton;
    run->setText("开始运行");
    run->setToolTip("启动自动程序");
    run->setFixedHeight(30);
 //   run->setFixedSize(200,40);
    run->setIcon(QIcon(":/programConfig/image/run.png"));
    connect(run,SIGNAL(clicked()),this,SLOT(runBtn_clicked()));
    Toplayout->addWidget(run);

    pause = new QPushButton;
    pause->setText("暂停");
    pause->setToolTip("暂停自动程序");
//    run->setFixedHeight(25);
    pause->setFixedSize(60,30);
 //   pause->setIcon(QIcon(":/programConfig/image/run.png"));
    connect(pause,SIGNAL(clicked()),this,SLOT(pauseBtn_clicked()));
    Toplayout->addWidget(pause);

    Toplayout->addStretch();

//    circulate = new QPushButton;
//    circulate->setText("0");
//    circulate->setToolTip("已完成的循环次数");
//    circulate->setFixedSize(60,40);
//    Toplayout->addWidget(circulate);
//    Toplayout->addStretch();

    QWidget *midWidget = new QWidget;
    midWidget->setFixedHeight(30);
    Vlayout->addWidget(midWidget);

    QHBoxLayout *Midlayout = new QHBoxLayout;
    Midlayout->setMargin(0);
    midWidget->setLayout(Midlayout);

    refresh = new QPushButton;
    refresh->setFixedSize(30,30);
    refresh->setToolTip("刷新配置列表");
    refresh->setIcon(QIcon(":/programConfig/image/refresh.png"));
    refresh->setIconSize(QSize(20,20));
    connect(refresh, &QPushButton::clicked, this, &ProgramList::refreshBtn_clicked);
    Midlayout->addWidget(refresh);

    nameList = new QComboBox;
    nameList->setView(new QListView);  //  必须加入，否则部分样式不生效
    connect(nameList,static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),this, &ProgramList::nameFile_changed);
    nameList->setFixedSize(120,26);
    nameList->setStyleSheet("QComboBox {border: 1px solid rgb(226,226,226);"
                                 "border-radius: 5px;"
                                 "padding: 1px 18px 1px 3px;"
                                 "background-color: rgb(255,255,255);"
                                 "font-family:SimHei;"
                                 "font-size:20px;}"
                                 "QComboBox::drop-down {subcontrol-origin: padding;"
                                 "subcontrol-position: top right;"
                                 "width:20px;"
                                 "padding-right:5px;"
                                 "border-left: none;}"
                                 "QComboBox::down-arrow {image: url(:/stateBar/image/arrow.png);"
                                 "width: 20px;"
                                 "height: 20px;}"
                                 "QComboBox QAbstractItemView::item{"
                                 "height:30px;"
                                 "font-family:SimHei;"
                                 "font-size:20px;}"
                                 "QComboBox QAbstractItemView::item {min-height: 30px;}"
                                 "QListView{font-family:SimHei;font-size:20px;}");

    Midlayout->addWidget(nameList);

    V2layout = new QVBoxLayout(w2);
    V2layout->setMargin(10);
    proList = new QListWidget;   // 子项目列表
    proList->setObjectName("proList");
    proList->setFocusPolicy(Qt::NoFocus);  //这禁用tab键和上下方向键并且除去复选框
    V2layout->addWidget(proList);

    obj = new ObjList;
    connect(obj, &ObjList::obj_clicked, this, &ProgramList::obj_clicked);
    Vlayout->addWidget(obj);
    connect(&timer, &QTimer::timeout, this, &ProgramList::timer_slot);
    connect(&runTimer, &QTimer::timeout, this, &ProgramList::runTimer_slot);

    QFile Stylefile(":/programConfig/programConfig.qss");
    if (Stylefile.exists()) {
        Stylefile.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(Stylefile.readAll());
        this->setStyleSheet(styleSheet);
        Stylefile.close();
    }
}

ProgramList::~ProgramList()
{
}

void ProgramList::state_change()
{
    refresh->setEnabled(state); // 刷新按钮恢复点击
    nameList->setEnabled(state); // 名称列表恢复点击
    if(state)   // 如果已经启动了
    {
       state = false;
       programCnt = 0;
       circulationCnt = 0;
       m_runIndex = 0;
       //circulate->setText(QString::number(circulationCnt));
       run->setIcon(QIcon(":/programConfig/image/run.png"));
       run->setText("开始运行");
       timer.stop();
       runTime = 0;
       runTimer.stop();
       obj->setAllBtn_Enable();
       m_bReady = true;
    }
    else
    {
       if(proList->count() == 0)
       {
           MyMessageBox msg(MyMessageBox::Critical, "提示", "请先添加配置！", MyMessageBox::Ok,this);
           msg.exec();
           return ;
       }
       state = true;
       programCnt = -1;
       m_runIndex = 0;
       run->setIcon(QIcon(":/programConfig/image/stop.png"));
       run->setText("停止运行");
       run->setToolTip("停止自动程序");
       timer.start(1000);
       runTimer.start(1000);
       pause->setText("暂停");
    }

    emit autoRun(state);
}

bool ProgramList::getState()
{
    return state;
}

void ProgramList::refreshBtn_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/config";
    QDir dir(path);

    if(!dir.exists())   //  检查目录是否存在
       return ;

    nameList->clear();
    auto fileInfo = dir.entryInfoList({ "*.txt" });
    if(fileInfo.size() <= 0)
    {
        proList->clear();  // 清除子列表
        obj->list_clear(); // 清除主列表
    }
    for (auto &itr : fileInfo)
    {
        nameList->addItem(itr.baseName());
    }
}

void ProgramList::runBtn_clicked()
{
    state_change();
}

void ProgramList::pauseBtn_clicked()
{
    if(pause->text() == "暂停")
    {
       if(timer.isActive())
       {
          pause->setText("继续");
          pause->setToolTip("继续自动程序");
          timer.stop();
          runTimer.stop();
       }
    }
    else
    {
       pause->setText("暂停");
       pause->setToolTip("暂停自动程序");
       timer.start();
       runTimer.start();
    }
}

void ProgramList::timer_slot()
{
	if (!m_bReady)
		return;
	 
     timer.stop();
     if(m_runIndex < total_obj)
     {
       sub_num = m_titles.at(m_runIndex).list.count();  // 获取子类数量
       obj->setRun(m_runIndex);
     }
     programCnt++;
     if(programCnt < sub_num)
     {
         proList->setCurrentRow(programCnt);
         if(proList->item(programCnt)->text().contains("---"))  //  不处理功能标签
         {
           timer.start(1000);
         }
         else if(proList->item(programCnt)->text().contains("延时"))
         {

             QStringList list = proList->item(programCnt)->text().split(" ");
             int delay = QString(list.at(1)).toDouble() * 60;  //  将分钟转换成秒钟
             if (delay <= 1)
                 delay = 1;

           timer.start(delay*1000);
       }
       else if(proList->item(programCnt)->text().contains("开始循环"))
       {
           QStringList list = proList->item(programCnt)->text().split(" ");
           totalcirculationCnt= QString(list.at(1)).toInt();  // 获取循环次数
           CirculateStartLine = programCnt;  // 保存循环起始位置
           timer.start(1000);
       }
       else if(proList->item(programCnt)->text().contains("结束循环"))
       {
           if(totalcirculationCnt > 0 && circulationCnt < totalcirculationCnt) // 判断循环次数大于0，且处于循环周期内
           {
              circulationCnt ++;
              if(circulationCnt == totalcirculationCnt)
              {
                circulationCnt = 0;
                totalcirculationCnt = 0;
  //              circulate->setText(QString::number(circulationCnt));
              }
              else
              {
                  programCnt = CirculateStartLine;
  //                circulate->setText(QString::number(circulationCnt));
              }
           }
           timer.start(1000);
       }
       else if(proList->item(programCnt)->text().contains("收集器"))
       {
           emit readyTorun_toColl(proList->item(programCnt)->text());
           timer.start(1000);
       }
       else
       {
            auto str = proList->item(programCnt)->text();	
            if (str.startsWith(tr("固体投料")))
                m_bReady = false;

       		emit readyTorun(str);
            timer.start(1000);
       }
       programCnt++;
       if(programCnt >= sub_num)
       {
           if(m_curIndex < (total_obj - 1)) // 判断最还在范围内
           {
              programCnt = -1;
              m_runIndex++;
           }
       }
     }
     else
     {
       state_change();
     }
}

void ProgramList::nameFile_changed(const QString& str)
{
    QString path = QCoreApplication::applicationDirPath()+"/config/"+str+".txt";

    QFile readFile(path);
    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;
    proList->clear();  // 清除子列表
    obj->list_clear(); // 清除主列表
    m_titles.clear(); // 清除之前的
    total_obj = 0; // 大类总数量
    totalCnt = 0;  // 子类总数量
    QTextStream stream(&readFile);    //  读取文件
    QString line;
    int flag = 0;
    while (!stream.atEnd())
    {
        ListTextContent list;
        list.list.clear();
        if(flag == 0)
        {
           line = stream.readLine();   //  逐行读取
        }
        else
        {
           flag = 0;
        }

        if(line.contains("---")) // 包含主类别项目
        {
            list.name = line;
            while(!stream.atEnd()) // 继续查询子类
            {
                line = stream.readLine();
                if(line.contains("---")) // 包含类别项目
                {
                   flag = 1;
                   break;
                }
                else
                {
                  list.list << line; // 记录子类别
                  totalCnt++;
                }
            }
            m_titles.append(list);
            obj->add_Btn(m_titles.at(total_obj).name, total_obj);
            total_obj++;
        }
        if (total_obj > 0)
            obj_clicked(0);

        if(line == NULL)
        {
           readFile.close();
           return ;
        }
    }
    readFile.close();
}

void ProgramList::runTimer_slot()
{
    runTime++;
    emit sendRunTime(runTime);
}

void ProgramList::clear_sub_list()
{
    while (QLayoutItem* item = V2layout->takeAt(0))
    {
        if (QWidget* widget = item->widget())
            widget->deleteLater();

        if (QSpacerItem* spaerItem = item->spacerItem())
            V2layout->removeItem(spaerItem);

        delete item;
    }
}

void ProgramList::obj_clicked(int index)
{
    proList->clear();
    proList->addItems(m_titles.at(index).list);
    m_curIndex = index;
    if (m_curIndex == m_runIndex)
        proList->setCurrentRow(programCnt);
}

void ProgramList::OnStoveTubeChanged(uint16_t type, uint16_t idx)
{
    if (programCnt < 0 || programCnt >= proList->count() || m_bReady)
        return;
    auto cmd = proList->item(programCnt)->text();
    if (cmd.startsWith(tr("固体投料")))
    {
        auto strlist = cmd.split(" ", QString::SkipEmptyParts);  //  以空格符分割
        if (strlist.size() <= 2)
            return;
        switch (type)
        {
        case FeederMgr::J_PrepareMate:
            if (strlist.at(1) == tr("配料"))
                m_bReady = true;
            break;
        case FeederMgr::J_StoveFixTube:
            if (strlist.at(1) == tr("装载炉膛") && strlist.at(2).toInt()-1 == idx)
                m_bReady = true;
            break;
        case FeederMgr::J_StoveTubeBack:
            if (strlist.at(1) == tr("收回反应管") && strlist.at(2).toInt()-1 == idx)
                m_bReady = true;
            break;
        }
    }
}
