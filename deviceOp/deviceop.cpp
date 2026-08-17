#include "deviceop.h"
#include "ui_deviceop.h"
#pragma execution_character_set("utf-8")

deviceOp::deviceOp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::deviceOp)
{
    ui->setupUi(this);
    ui->widget_32->hide();
    ui->widget->hide();
    ui->pump_clean->hide();
    ui->pump_clean_2->hide();

    ui->Stove->setTableFormat(3,PRO_CNT);   //  初始化表格
    QString name = ui->Stove->getTitleName();
    ui->Stove->setTitle(name);  // 保存数据文件名
    ui->Stove->Table_init();
    connect(ui->Stove,&stove::stove_change,this,[=](int who){
        stove_No = who;
    });

    connect(ui->Stove, &stove::stove_refresh, this, &deviceOp::stove_refresh);
    connect(ui->Stove, &stove::StoveTempRise, this, &deviceOp::readyTorun);
    connect(ui->Stove, &stove::StoveTempFalling, this, &deviceOp::readyTorun);

    ui->pres_on->setIcon(QIcon(":/deviceOp/image/min.png"));
    ui->pres_on->setIconSize(QSize(36,36));
    ui->pres_on_2->setIcon(QIcon(":/deviceOp/image/min.png"));
    ui->pres_on_2->setIconSize(QSize(36,36));

    ui->pres_off->setIcon(QIcon(":/deviceOp/image/max.png"));
    ui->pres_off->setIconSize(QSize(36,36));
    ui->pres_off_2->setIcon(QIcon(":/deviceOp/image/max.png"));
    ui->pres_off_2->setIconSize(QSize(36,36));

    ui->pres_inc->setIcon(QIcon(":/deviceOp/image/inc.png"));
    ui->pres_inc->setIconSize(QSize(36,36));
    ui->pres_inc_2->setIcon(QIcon(":/deviceOp/image/inc.png"));
    ui->pres_inc_2->setIconSize(QSize(36,36));

    ui->pres_dec->setIcon(QIcon(":/deviceOp/image/dec.png"));
    ui->pres_dec->setIconSize(QSize(36,36));
    ui->pres_dec_2->setIcon(QIcon(":/deviceOp/image/dec.png"));
    ui->pres_dec_2->setIconSize(QSize(36,36));

    ui->temp1_Btn->setIcon(QIcon(":/deviceOp/image/set.png"));
    ui->pump_run->setIcon(QIcon(":/deviceOp/image/set.png"));
    ui->pump_run_2->setIcon(QIcon(":/deviceOp/image/set.png"));
    ui->flow1btn->setIcon(QIcon(":/deviceOp/image/set.png"));
    ui->flow2btn->setIcon(QIcon(":/deviceOp/image/set.png"));

    // 默认自动模式
    ui->widget_9->setEnabled(false);
    ui->widget_23->setEnabled(false);
    // 默认不显示
    ui->widget_2->hide();

    ui->temp1Adj->setValidator(new QIntValidator(0, 999, this));

    connect(this, &deviceOp::selfcmdTorun, this, &deviceOp::check_run);

//    QString comstyle = R"(
//            QComboBox {
//                background-color: transparent;
//                color: black;
//                border: 1px solid #c0c0c0;
//                border-radius: 6px;
//                padding: 4px 8px;
//                font-size: 18px;
//                min-height: 28px;
//            }

//            QComboBox:hover {
//                border: 1px solid #a0a0a0;
//            }

//            QComboBox:focus {
//                border: 1px solid #785093;
//                background-color: #f8f8f8;
//            }

//            QComboBox::drop-down {
//                border: none;
//                background: transparent;
//                width: 28px;
//            }

//            QComboBox::down-arrow {
//                border-left: 5px solid transparent;
//                border-right: 5px solid transparent;
//                border-top: 8px solid #606060;
//                width: 0;
//                height: 0;
//                margin-right: 8px;
//            }

//            QComboBox QAbstractItemView {
//                background-color: white;
//                border: 1px solid #c0c0c0;
//                border-radius: 4px;
//                selection-background-color: #785093;
//                selection-color: white;
//                outline: none;
//            }

//            QComboBox QAbstractItemView::item {
//                padding: 6px 8px;
//                font-size: 18px;
//                color: #333333;
//                min-height: 28px;
//            }

//            QComboBox QAbstractItemView::item:hover {
//                background-color: #f0f0f0;
//            }

//            QComboBox QAbstractItemView::item:selected {
//                background-color: #785093;
//                color: white;
//            }
//        )";
//    ui->pumpBox->setStyleSheet(comstyle);
//    ui->valveBox->setStyleSheet(comstyle);
}

deviceOp::~deviceOp()
{
    delete ui;
}

void deviceOp::update_user_set(bool state)
{
    if(state)
    {
        user_state = true;
    }
}

void deviceOp::autoRun(bool state)
{
    auto_Run = state;
    ui->Stove->autoState(state);
}

void deviceOp::updateInfo(ReceiveData *data)
{
    ui->Stove->setTime(QString::number(data->ProgTime[stove_No] / 3600) + ":" + QString::number((data->ProgTime[stove_No]%3600) / 60) + ":" + QString::number(data->ProgTime[stove_No] % 60));
    ui->Stove->setTemp(QString::number(data->Aimtemp[stove_No]) + "℃");
    ui->Stove->setStep(QString::number(data->ProgStep[stove_No]));

    if(data->adjState[stove_No])
    {
        ui->temp1Adj_btn->setText("整定中");
    }
    else
    {
        ui->temp1Adj_btn->setText("整定");
    }

    ui->pres_val->setText(QString::number(data->pres[6]) + "MPa");
    ui->pres_val_2->setText(QString::number(data->pres[7]) + "MPa");

//    if(user_state)
//    {
//        ui->flow1_range->setText(QString::number(data->Flow_range[0]));
//        ui->flow2_range->setText(QString::number(data->Flow_range[1]));
//        ui->pump_cali->setText(QString::number(data->pump_cali[0]));
//        ui->pump_cali_2->setText(QString::number(data->pump_cali[1]));
//        user_state = false;
//    }
}

/*程序段各单元格更新时触发重新设置（不拦截）*/
void deviceOp::stove_refresh(int who,int row, int col, float val)
{
   if(col == 1)  // 温度
   {
       val = val * 10;
       QString str = "程序温度 "+QString::number(who)+" "+QString::number(row)+" 设置为 "+QString::number(val)+" ℃";
       emit cmdTorun(str);
   }
   else if(col == 2)  // 时间
   {
       val = val * 10;
       QString str = "程序时间 "+QString::number(who)+" "+QString::number(row)+" 设置为 "+QString::number(val)+" min";
       emit cmdTorun(str);
   }
}

void deviceOp::tempAdjustEnable()
{
    ui->widget_2->show();
}

//void deviceOp::set_stove_name(QString name)
//{
//   stove_name = name;
//}

//void deviceOp::StoveTable_init()
//{
//    QString path = QCoreApplication::applicationDirPath()+"/user";
//    QDir dir(path);
//    if(!dir.exists())   //  检查目录是否存在
//    {
//       dir.mkdir(path);
//    }
//    path = path + "/"+stove_name;
//    file.setFileName(path);    //  读取文件中的数据
//    if(file.exists())   //  如果文件存在
//    {
//        file.open(QFile::ReadWrite);
//        file.read((char *)&table,sizeof(table));
//        file.close();
//    }
//    else {
//        file.open(QFile::ReadWrite);
//        for(int i = 0;i<10;i++)
//        {
//          table.temp[i] = 0;
//          table.time[i] = 0;
//        }
//        file.write((char *)&table,sizeof(table));
//        file.close();
//    }
//}


void deviceOp::readyTorun(const QString& str)
{
    emit cmdTorun(str);
}


//void deviceOp::on_tempBtn1_clicked()
//{
//    if(ui->temp1->text().isEmpty())
//    {
//        return ;
//    }

//    emit cmdTorun("汽化炉 设置为 "+ ui->temp1->text() + " ℃");
//    MyMessageBox msg(MyMessageBox::Success, "提示", "已设定", MyMessageBox::Ok,this);
//    msg.exec();
//}

#if 0
void deviceOp::on_temp1_editingFinished()
{
     int temp = ui->temp1->text().toDouble();
     if(temp > 999)
     {
       temp = 999;
     }
    table.temp[0] = temp;

     QString str = "反应炉程序温度 0 设置为 "+QString::number(temp)+" ℃";
     emit cmdTorun(str);
}

void deviceOp::on_temp2_editingFinished()
{
    int temp = ui->temp2->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[1] = temp;

    QString str = "反应炉程序温度 1 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp3_editingFinished()
{
    int temp = ui->temp3->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[2] = temp;

    QString str = "反应炉程序温度 2 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp4_editingFinished()
{
    int temp = ui->temp4->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[3] = temp;

    QString str = "反应炉程序温度 3 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp5_editingFinished()
{
    int temp = ui->temp5->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[4] = temp;

    QString str = "反应炉程序温度 4 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp6_editingFinished()
{
    int temp = ui->temp6->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[5] = temp;

    QString str = "反应炉程序温度 5 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp7_editingFinished()
{
    int temp = ui->temp7->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[6] = temp;

    QString str = "反应炉程序温度 6 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp8_editingFinished()
{
    int temp = ui->temp8->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[7] = temp;

    QString str = "反应炉程序温度 7 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp9_editingFinished()
{
    int temp = ui->temp9->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[8] = temp;

    QString str = "反应炉程序温度 8 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}

void deviceOp::on_temp10_editingFinished()
{
    int temp = ui->temp10->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.temp[9] = temp;

    QString str = "反应炉程序温度 9 设置为 "+QString::number(temp)+" ℃";
    emit cmdTorun(str);
}


void deviceOp::on_time1_editingFinished()
{
    int temp = ui->time1->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[0] = temp;

    QString str = "反应炉程序时间 0 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time2_editingFinished()
{
    int temp = ui->time2->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[1] = temp;

    QString str = "反应炉程序时间 1 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time3_editingFinished()
{
    int temp = ui->time3->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[2] = temp;

    QString str = "反应炉程序时间 2 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time4_editingFinished()
{
    int temp = ui->time4->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[3] = temp;

    QString str = "反应炉程序时间 3 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time5_editingFinished()
{
    int temp = ui->time5->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[4] = temp;

    QString str = "反应炉程序时间 4 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time6_editingFinished()
{
    int temp = ui->time6->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[5] = temp;

    QString str = "反应炉程序时间 5 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time7_editingFinished()
{
    int temp = ui->time7->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[6] = temp;

    QString str = "反应炉程序时间 6 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time8_editingFinished()
{
    int temp = ui->time8->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[7] = temp;

    QString str = "反应炉程序时间 7 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time9_editingFinished()
{
    int temp = ui->time9->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[8] = temp;

    QString str = "反应炉程序时间 8 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_time10_editingFinished()
{
    int temp = ui->time10->text().toDouble();
    if(temp > 999)
    {
      temp = 999;
    }
    table.time[9] = temp;

    QString str = "反应炉程序时间 9 设置为 "+QString::number(temp)+" min";
    emit cmdTorun(str);
}

void deviceOp::on_run_clicked()
{
    int i;
    QString cmd = "反应炉程序升温 ";
    for(i=0;i<10;i++)
    {
       cmd = cmd + QString::number(table.temp[i])+ " ";
       cmd = cmd + QString::number(table.time[i])+ " ";
    }
    emit cmdTorun(cmd);

    MyMessageBox msg(MyMessageBox::Success, "提示", "开始运行", MyMessageBox::Ok,this);
    msg.exec();
}

void deviceOp::on_stop_clicked()
{
    emit cmdTorun("反应炉 降温");

    MyMessageBox msg(MyMessageBox::Success, "提示", "停止运行", MyMessageBox::Ok,this);
    msg.exec();
}

void deviceOp::on_save_clicked()
{
    file.open(QFile::ReadWrite);
    file.write((const char *)&table,sizeof(table));
    file.close();

    MyMessageBox msg(MyMessageBox::Success, "提示", "已保存", MyMessageBox::Ok,this);
    msg.exec();
}
#endif

void deviceOp::on_temp1Adj_btn_clicked()
{
    if(ui->temp1Adj_btn->text() == "整定中")
    {
        QString str = "反应炉 "+QString::number(stove_No)+" 停止整定";
        emit cmdTorun(str);
        MyMessageBox msg(MyMessageBox::Success, "提示", "停止整定", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }

    if(ui->temp1Adj_btn->text() == "整定")
    {
        if(ui->temp1Adj->text().isEmpty() || ui->temp1Adj->text().toInt() == 0)
        {
            return ;
        }

        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"注意!整定开始?","提示",MyMessageBox::Yes | MyMessageBox::No);
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

        QString str = "反应炉 "+QString::number(stove_No)+" 开始整定 "+ ui->temp1Adj->text() + " ℃";
        emit cmdTorun(str);
        MyMessageBox msg(MyMessageBox::Success, "提示", "开始整定", MyMessageBox::Ok,this);
        msg.exec();
    }
}

void deviceOp::on_pres_auto_clicked()
{
   if(ui->pres_auto->text() == "自动")
   {
       MyMessageBox:: StandardButton result = MyMessageBox::information(this,"切换至手动?","提示",MyMessageBox::Yes | MyMessageBox::No);
       switch (result)
       {
           case MyMessageBox::Yes:
                ui->pres_auto->setText("手动");
                ui->widget_9->setEnabled(true);
                cmdTorun("背压阀 0 模式 手动 1");
                break;
           case MyMessageBox::No:

                break;
           default:
                break;
       }
   }
   else
   {
       MyMessageBox:: StandardButton result = MyMessageBox::information(this,"切换至自动?","提示",MyMessageBox::Yes | MyMessageBox::No);
       switch (result)
       {
           case MyMessageBox::Yes:

                if(ui->label_on->text() == "停止" || ui->label_off->text() == "停止")
                {
                    MyMessageBox msg(MyMessageBox::Success, "提示", "请先停止阀动作", MyMessageBox::Ok,this);
                    msg.exec();
                    return ;
                }

                ui->pres_auto->setText("自动");
                ui->widget_9->setEnabled(false);
                cmdTorun("背压阀 0 模式 自动 0");
                break;
           case MyMessageBox::No:

                break;
           default:
                break;
       }
   }
}
void deviceOp::on_pres_auto_2_clicked()
{
   if(ui->pres_auto_2->text() == "自动")
   {
       MyMessageBox:: StandardButton result = MyMessageBox::information(this,"切换至手动?","提示",MyMessageBox::Yes | MyMessageBox::No);
       switch (result)
       {
           case MyMessageBox::Yes:
                ui->pres_auto_2->setText("手动");
                ui->widget_23->setEnabled(true);
                cmdTorun("背压阀 1 模式 手动 1");
                break;
           case MyMessageBox::No:

                break;
           default:
                break;
       }
   }
   else
   {
       MyMessageBox:: StandardButton result = MyMessageBox::information(this,"切换至自动?","提示",MyMessageBox::Yes | MyMessageBox::No);
       switch (result)
       {
           case MyMessageBox::Yes:

                if(ui->label_on_2->text() == "停止" || ui->label_off_2->text() == "停止")
                {
                    MyMessageBox msg(MyMessageBox::Success, "提示", "请先停止阀动作", MyMessageBox::Ok,this);
                    msg.exec();
                    return ;
                }

                ui->pres_auto_2->setText("自动");
                ui->widget_23->setEnabled(false);
                cmdTorun("背压阀 1 模式 自动 0");
                break;
           case MyMessageBox::No:

                break;
           default:
                break;
       }
   }
}

void deviceOp::on_pres_on_clicked()
{
    if(ui->label_on->text() == "全开")
    {
        cmdTorun("背压阀 0 状态设置为 全开 1");
        ui->label_on->setText("停止");

        ui->pres_off->setEnabled(false);
        ui->pres_dec->setEnabled(false);
        ui->pres_inc->setEnabled(false);

        MyMessageBox msg(MyMessageBox::Success, "提示", "启动全开", MyMessageBox::Ok,this);
        msg.exec();
    }
    else
    {
        cmdTorun("背压阀 0 状态设置为 停止 0");
        ui->label_on->setText("全开");

        ui->pres_off->setEnabled(true);
        ui->pres_dec->setEnabled(true);
        ui->pres_inc->setEnabled(true);

        MyMessageBox msg(MyMessageBox::Success, "提示", "停止", MyMessageBox::Ok,this);
        msg.exec();
    }
}
void deviceOp::on_pres_on_2_clicked()
{
    if(ui->label_on_2->text() == "全开")
    {
        cmdTorun("背压阀 1 状态设置为 全开 1");
        ui->label_on_2->setText("停止");

        ui->pres_off_2->setEnabled(false);
        ui->pres_dec_2->setEnabled(false);
        ui->pres_inc_2->setEnabled(false);

        MyMessageBox msg(MyMessageBox::Success, "提示", "启动全开", MyMessageBox::Ok,this);
        msg.exec();
    }
    else
    {
        cmdTorun("背压阀 1 状态设置为 停止 0");
        ui->label_on_2->setText("全开");

        ui->pres_off_2->setEnabled(true);
        ui->pres_dec_2->setEnabled(true);
        ui->pres_inc_2->setEnabled(true);

        MyMessageBox msg(MyMessageBox::Success, "提示", "停止", MyMessageBox::Ok,this);
        msg.exec();
    }
}

void deviceOp::on_pres_off_clicked()
{
    if(ui->label_off->text() == "全关")
    {
        cmdTorun("背压阀 0 状态设置为 全关 2");
        ui->label_off->setText("停止");

        ui->pres_on->setEnabled(false);
        ui->pres_dec->setEnabled(false);
        ui->pres_inc->setEnabled(false);

        MyMessageBox msg(MyMessageBox::Success, "提示", "启动全关", MyMessageBox::Ok,this);
        msg.exec();
    }
    else
    {
        cmdTorun("背压阀 0 状态设置为 停止 0");
        ui->label_off->setText("全关");

        ui->pres_on->setEnabled(true);
        ui->pres_dec->setEnabled(true);
        ui->pres_inc->setEnabled(true);

        MyMessageBox msg(MyMessageBox::Success, "提示", "停止", MyMessageBox::Ok,this);
        msg.exec();
    }
}
void deviceOp::on_pres_off_2_clicked()
{
    if(ui->label_off_2->text() == "全关")
    {
        cmdTorun("背压阀 1 状态设置为 全关 2");
        ui->label_off_2->setText("停止");

        ui->pres_on_2->setEnabled(false);
        ui->pres_dec_2->setEnabled(false);
        ui->pres_inc_2->setEnabled(false);

        MyMessageBox msg(MyMessageBox::Success, "提示", "启动全关", MyMessageBox::Ok,this);
        msg.exec();
    }
    else
    {
        cmdTorun("背压阀 1 状态设置为 停止 0");
        ui->label_off_2->setText("全关");

        ui->pres_on_2->setEnabled(true);
        ui->pres_dec_2->setEnabled(true);
        ui->pres_inc_2->setEnabled(true);

        MyMessageBox msg(MyMessageBox::Success, "提示", "停止", MyMessageBox::Ok,this);
        msg.exec();
    }
}

void deviceOp::on_pres_inc_pressed()
{
     cmdTorun("背压阀 0 状态设置为 全关 2");
}
void deviceOp::on_pres_inc_2_pressed()
{
     cmdTorun("背压阀 1 状态设置为 全关 2");
}

void deviceOp::on_pres_inc_released()
{
     cmdTorun("背压阀 0 状态设置为 停止 0");
}
void deviceOp::on_pres_inc_2_released()
{
     cmdTorun("背压阀 1 状态设置为 停止 0");
}

void deviceOp::on_pres_dec_pressed()
{
     cmdTorun("背压阀 0 状态设置为 全开 1");
}
void deviceOp::on_pres_dec_2_pressed()
{
     cmdTorun("背压阀 1 状态设置为 全开 1");
}

void deviceOp::on_pres_dec_released()
{
     cmdTorun("背压阀 0 状态设置为 停止 0");
}
void deviceOp::on_pres_dec_2_released()
{
     cmdTorun("背压阀 1 状态设置为 停止 0");
}

void deviceOp::on_flow1btn_clicked()
{
     if(ui->flow1_range->text().isEmpty())
     {
         return ;
     }

     selfcmdTorun("流量计 0 流量设置为 " + ui->flow1_range->text() + " ml/min");
}
void deviceOp::on_flow2btn_clicked()
{
     if(ui->flow2_range->text().isEmpty())
     {
         return ;
     }

     selfcmdTorun("流量计 1 流量设置为 " + ui->flow2_range->text() + " ml/min");
}

void deviceOp::on_flow1_sw_clicked()
{
    if(ui->flow1_sw->text() == "短接")
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"流量计短接?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->flow1_sw->setText("短接中");
                 cmdTorun("流量计 0 短接 开启");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
    else
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"流量计关闭短接?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->flow1_sw->setText("短接");
                 cmdTorun("流量计 0 短接 关闭");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
}

void deviceOp::on_flow2_sw_clicked()
{
    if(ui->flow2_sw->text() == "短接")
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"流量计短接?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->flow2_sw->setText("短接中");
                 cmdTorun("流量计 1 短接 开启");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
    else
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"流量计关闭短接?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->flow2_sw->setText("短接");
                 cmdTorun("流量计 1 短接 关闭");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
}

void deviceOp::on_pump_clean_clicked()
{
    if(ui->pump_clean->text() == "启动清洗")
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"启动清洗?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->pump_clean->setText("清洗中");
                 cmdTorun("柱塞泵 0 清洗 启动 2");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
    else
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"关闭清洗?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->pump_clean->setText("启动清洗");
                 cmdTorun("柱塞泵 0 清洗 关闭 0");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
}
void deviceOp::on_pump_run_clicked()
{
//    if(ui->pump_run->text() == "开始运行")
//    {
//        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"运行程序?","提示",MyMessageBox::Yes | MyMessageBox::No);
//        switch (result)
//        {
//            case MyMessageBox::Yes:

//                 ui->pump_run->setText("运行中");
//                 cmdTorun("柱塞泵 0 运行 启动 1");
//                 break;
//            case MyMessageBox::No:

//                 break;
//            default:
//                 break;
//        }
//    }
//    else
//    {
//        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"关闭程序?","提示",MyMessageBox::Yes | MyMessageBox::No);
//        switch (result)
//        {
//            case MyMessageBox::Yes:

//                 ui->pump_run->setText("开始运行");
//                 cmdTorun("柱塞泵 0 运行 关闭 0");
//                 break;
//            case MyMessageBox::No:

//                 break;
//            default:
//                 break;
//        }
//    }

    if(ui->pump_cali->text().isEmpty())
    {
        return ;
    }

    selfcmdTorun("柱塞泵 0 流量设置为 " + ui->pump_cali->text() + " ml/min");
}
void deviceOp::on_pump_clean_2_clicked()
{
    if(ui->pump_clean_2->text() == "启动清洗")
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"启动清洗?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->pump_clean_2->setText("清洗中");
                 cmdTorun("柱塞泵 1 清洗 启动 2");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
    else
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"关闭清洗?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:

                 ui->pump_clean_2->setText("启动清洗");
                 cmdTorun("柱塞泵 1 清洗 关闭 0");
                 break;
            case MyMessageBox::No:

                 break;
            default:
                 break;
        }
    }
}
void deviceOp::on_pump_run_2_clicked()
{
//    if(ui->pump_run_2->text() == "开始运行")
//    {
//        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"运行程序?","提示",MyMessageBox::Yes | MyMessageBox::No);
//        switch (result)
//        {
//            case MyMessageBox::Yes:

//                 ui->pump_run_2->setText("运行中");
//                 cmdTorun("柱塞泵 1 运行 启动 1");
//                 break;
//            case MyMessageBox::No:

//                 break;
//            default:
//                 break;
//        }
//    }
//    else
//    {
//        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"关闭程序?","提示",MyMessageBox::Yes | MyMessageBox::No);
//        switch (result)
//        {
//            case MyMessageBox::Yes:

//                 ui->pump_run_2->setText("开始运行");
//                 cmdTorun("柱塞泵 1 运行 关闭 0");
//                 break;
//            case MyMessageBox::No:

//                 break;
//            default:
//                 break;
//        }
//    }

    if(ui->pump_cali_2->text().isEmpty())
    {
        return ;
    }

    selfcmdTorun("柱塞泵 1 流量设置为 " + ui->pump_cali_2->text() + " ml/min");
}

//void deviceOp::on_flow1_range_editingFinished()
//{
//     if(ui->flow1_range->text().isEmpty())
//     {
//         return ;
//     }
//     MyMessageBox:: StandardButton result = MyMessageBox::information(this,"设置流量计量程?","注意!!!",MyMessageBox::Yes | MyMessageBox::No);
//     switch (result)
//     {
//         case MyMessageBox::Yes:
//              break;
//         case MyMessageBox::No:
//         default:
//              return ;
//     }
//     cmdTorun("流量计 0 量程设置为 " + ui->flow1_range->text() + " ml/min");
//}

//void deviceOp::on_flow2_range_editingFinished()
//{
//    if(ui->flow2_range->text().isEmpty())
//    {
//        return ;
//    }
//    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"设置流量计量程?","注意!!!",MyMessageBox::Yes | MyMessageBox::No);
//    switch (result)
//    {
//        case MyMessageBox::Yes:
//             break;
//        case MyMessageBox::No:
//        default:
//             return ;
//    }
//    cmdTorun("流量计 1 量程设置为 " + ui->flow2_range->text() + " ml/min");
//}

//void deviceOp::on_pump_cali_editingFinished()
//{
//    if(ui->pump_cali->text().isEmpty())
//    {
//        return ;
//    }

//    double temp = ui->pump_cali->text().toDouble();

//    if(temp == 0)
//    {
//       return ;
//    }

//    if(temp > 2)
//    {
//       temp = 2;
//    }
//    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"设置柱塞泵校准系数?","注意!!!",MyMessageBox::Yes | MyMessageBox::No);
//    switch (result)
//    {
//        case MyMessageBox::Yes:
//             break;
//        case MyMessageBox::No:
//        default:
//             return ;
//    }
//    cmdTorun("柱塞泵 0 校准系数设置为 " + QString::number(temp));
//}

//void deviceOp::on_pump_cali_2_editingFinished()
//{
//    if(ui->pump_cali_2->text().isEmpty())
//    {
//        return ;
//    }

//    double temp = ui->pump_cali_2->text().toDouble();

//    if(temp == 0)
//    {
//       return ;
//    }

//    if(temp > 2)
//    {
//       temp = 2;
//    }
//    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"设置柱塞泵校准系数?","注意!!!",MyMessageBox::Yes | MyMessageBox::No);
//    switch (result)
//    {
//        case MyMessageBox::Yes:
//             break;
//        case MyMessageBox::No:
//        default:
//             return ;
//    }
//    cmdTorun("柱塞泵 1 校准系数设置为 " + QString::number(temp));
//}

bool deviceOp::check_run(QString str)
{
    if(auto_Run)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("确认");

        // 先清空所有样式
        msgBox.setStyleSheet("");

        // 重新设置完整样式
        msgBox.setStyleSheet(R"(
            QMessageBox, QMessageBox * {
                background-color: white;
                color: black;
                font-size: 14px;
            }

            QPushButton {
                background-color: #785093;
                color: black;
                border: 1px solid #cccccc;
                border-radius: 4px;
                padding: 6px 12px;
                min-width: 80px;
            }

            QPushButton:hover {
                background-color: #0080ff;
                color: white;
            }

            QPushButton#qt_msgbox_yes {
                background-color: #00ffff;
            }

            QPushButton#qt_msgbox_no {
                background-color: #785093;
            }
        )");

        msgBox.setText(QString("是否继续执行命令：\n%1").arg(str));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if(msgBox.exec() != QMessageBox::Yes)
        {
            return false;
        }
    }
    cmdTorun(str);

    return true;
}
