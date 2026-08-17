#include "projectmode.h"
#include "ui_projectmode.h"
#pragma execution_character_set("utf-8")

projectMode::projectMode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::projectMode)
{
    ui->setupUi(this);

    ui->update->setIcon(QIcon(":/projectMode/image/refresh.png"));
    ui->update->setIconSize(QSize(25,25));

    ui->update_plugin->setIcon(QIcon(":/projectMode/image/refresh.png"));
    ui->update_plugin->setIconSize(QSize(25,25));

    ui->state->setIcon(QIcon(":/projectMode/image/refresh.png"));
    ui->state->setIconSize(QSize(25,25));
}

projectMode::~projectMode()
{
    delete ui;
}

void projectMode::updateInfo(ReceiveData *data)
{
    if(user_state)
    {
        ui->flow1_range->setText(QString::number(data->Flow_range[0]));
        ui->flow2_range->setText(QString::number(data->Flow_range[1]));
        ui->liquid_range->setText(QString::number(data->liquid_range[0]));
        ui->liquid2_range->setText(QString::number(data->liquid_range[1]));
        ui->pres1_range->setText(QString::number(data->pres_range[0]));
        ui->pres2_range->setText(QString::number(data->pres_range[1]));
        ui->pres3_range->setText(QString::number(data->pres_range[2]));
        ui->pres4_range->setText(QString::number(data->pres_range[3]));
        ui->pump_cali->setText(QString::number(data->pump_cali[0]));
        ui->pump2_cali->setText(QString::number(data->pump_cali[1]));
        user_state = false;
    }
}

void projectMode::update_user_set(bool state)
{
    if(state)
    {
        user_state = true;
    }
}

void projectMode::on_flow1_range_editingFinished()
{
     if(ui->flow1_range->text().isEmpty())
     {
        return ;
     }

     cmdTorun("流量计 0 量程设置为 " + ui->flow1_range->text() + " ml/min");
}
void projectMode::on_flow2_range_editingFinished()
{
     if(ui->flow2_range->text().isEmpty())
     {
        return ;
     }

     cmdTorun("流量计 1 量程设置为 " + ui->flow2_range->text() + " ml/min");
}

void projectMode::on_liquid_range_editingFinished()
{
     if(ui->liquid_range->text().isEmpty())
     {
        return ;
     }

     cmdTorun("液位计量程 0 设置为 " + ui->liquid_range->text() + " mm");
}
void projectMode::on_liquid2_range_editingFinished()
{
     if(ui->liquid2_range->text().isEmpty())
     {
        return ;
     }

     cmdTorun("液位计量程 1 设置为 " + ui->liquid2_range->text() + " mm");
}

void projectMode::on_pump_cali_editingFinished()
{
    if(ui->pump_cali->text().isEmpty())
    {
        return ;
    }

    double temp = ui->pump_cali->text().toDouble();

    if(temp == 0)
    {
       return ;
    }

    if(temp > 2)
    {
       temp = 2;
    }
//    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"设置柱塞泵校准系数?","注意!!!",MyMessageBox::Yes | MyMessageBox::No);
//    switch (result)
//    {
//        case MyMessageBox::Yes:
//             break;
//        case MyMessageBox::No:
//        default:
//             return ;
//    }
    cmdTorun("柱塞泵 0 校准系数设置为 " + QString::number(temp));
}

void projectMode::on_pump2_cali_editingFinished()
{
    if(ui->pump2_cali->text().isEmpty())
    {
        return ;
    }

    double temp = ui->pump2_cali->text().toDouble();

    if(temp == 0)
    {
       return ;
    }

    if(temp > 2)
    {
       temp = 2;
    }
//    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"设置柱塞泵校准系数?","注意!!!",MyMessageBox::Yes | MyMessageBox::No);
//    switch (result)
//    {
//        case MyMessageBox::Yes:
//             break;
//        case MyMessageBox::No:
//        default:
//             return ;
//    }
    cmdTorun("柱塞泵 1 校准系数设置为 " + QString::number(temp));
}

void projectMode::on_pres1_range_editingFinished()
{
    if(ui->pres1_range->text().isEmpty())
    {
       return ;
    }

    cmdTorun("压力量程 0 设置为 " + ui->pres1_range->text() + " MPa");
}

void projectMode::on_pres2_range_editingFinished()
{
    if(ui->pres2_range->text().isEmpty())
    {
       return ;
    }

    cmdTorun("压力量程 1 设置为 " + ui->pres2_range->text() + " MPa");
}

void projectMode::on_pres3_range_editingFinished()
{
    if(ui->pres3_range->text().isEmpty())
    {
       return ;
    }

    cmdTorun("压力量程 2 设置为 " + ui->pres3_range->text() + " MPa");
}

void projectMode::on_pres4_range_editingFinished()
{
    if(ui->pres4_range->text().isEmpty())
    {
       return ;
    }

    cmdTorun("压力量程 3 设置为 " + ui->pres4_range->text() + " MPa");
}

void projectMode::on_update_clicked()
{
#ifdef Q_OS_WIN
    MyMessageBox msg(MyMessageBox::Success, "提示", "程序更新失败", MyMessageBox::Ok,this);
    msg.exec();
#else
     int ret = 0;
     QString cmd;
     cmd = "mv /run/media/sda1/HS_H_Flow /huasi";
     ret = system(cmd.toStdString().c_str()) ;
     if(ret == 0)
     {
       cmd = "sync";
       system(cmd.toStdString().c_str()) ;
       MyMessageBox msg(MyMessageBox::Success, "提示", "程序更新成功", MyMessageBox::Ok,this);
       msg.exec();
     }
     else
     {
         MyMessageBox msg(MyMessageBox::Success, "提示", "程序更新失败", MyMessageBox::Ok,this);
         msg.exec();
     }
#endif
}

void projectMode::on_update_plugin_clicked()
{
#ifdef Q_OS_WIN
    MyMessageBox msg(MyMessageBox::Success, "提示", "插件更新失败", MyMessageBox::Ok,this);
    msg.exec();
#else
     int ret = 0;
     QString cmd;
     cmd = "mv /run/media/sda1/libQt5SoftKeyboard.so /usr/lib/plugins/platforminputcontexts";
     ret = system(cmd.toStdString().c_str()) ;
     if(ret == 0)
     {
         cmd = "sync";
         system(cmd.toStdString().c_str()) ;
         MyMessageBox msg(MyMessageBox::Success, "提示", "插件更新成功", MyMessageBox::Ok,this);
         msg.exec();
     }
     else
     {
         MyMessageBox msg(MyMessageBox::Success, "提示", "插件更新失败", MyMessageBox::Ok,this);
         msg.exec();
     }
#endif
}

void projectMode::on_state_clicked()
{
    if(ui->dev->text().isEmpty())
    {
        return;
    }

    int dev = ui->dev->text().toInt();
    if(dev>=1 && dev<=4)
    {
        MyMessageBox:: StandardButton result = MyMessageBox::information(this,"是否更新切换阀状态?","提示",MyMessageBox::Yes | MyMessageBox::No);
        switch (result)
        {
            case MyMessageBox::Yes:
                 break;
            case MyMessageBox::No:
            default:
                 return;
        }
        QString str;
        str = "电动四通阀 "+QString::number(dev+1)+" 设置为 开";
        cmdTorun(str);
    }
}
