#include "deviceop.h"
#include <QFile>
#include <QDir>
#include <QValidator>
#include <QTextStream>
#include "HsApplication.h"
#include "DevContrlMgr/portthread.h"

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
    QTimer::singleShot(10, this, [=] {
        connect(hsApp->getThread(), &portThread::dateReceived, this, &deviceOp::updateInfo);
    });
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

void deviceOp::readyTorun(const QString& str)
{
    emit cmdTorun(str);
}

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
