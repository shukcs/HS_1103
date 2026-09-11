#include "diagram.h"
#include <QScreen>
#include <QTimer>
#include <QMessageBox>

#include "strDecoder/portthread.h"
#include "flowset.h"

#include "ui_diagram.h"
#pragma execution_character_set("utf-8")

diagram::diagram(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::diagram)
{
    ui->setupUi(this);

    for(int i=0;i<SW_NUM;i++)
    {
       swState[i] = false;
    }
    for(int i=0;i<FLOW_NUM;i++)
    {
       flow_sw[i] = false;
       flow_set[i] = 0;
    }
    for(int i=0;i<PUMP_NUM;i++)
    {
        pump_flow[i] = 0;
    }
    for(int i=0;i<PRESS_NUM;i++)
    {
        pres_set[i] = 0;
    }

    //ui->pres3->hide();
    //ui->pres7->hide();
    initSlots();
}

diagram::~diagram()
{
    delete ui;
}

void diagram::updateInfo(ReceiveData *data)
{
    if(update_flg)
    {
        update_flg = false;
        for(uint8_t i=0;i<SWITCH_NUM;i++)
        {
           swIconCtrl(ACTSW_NUM+i, data->Switch_state[i]);
        }
    }

    ui->flow1->setText(QString::number(data->Flow[0]) + "ml/min");
    ui->flow2->setText(QString::number(data->Flow[1]) + "ml/min");

    ui->pres1->setText(QString::number(data->pres[0]) + "MPa");
    ui->pres2->setText(QString::number(data->pres[1]) + "MPa");
    ui->pres3->setText(QString::number(data->pres[4]) + "MPa");
    ui->pres4->setText(QString::number(data->pres[6]) + "MPa");
    ui->pres5->setText(QString::number(data->pres[2]) + "MPa");
    ui->pres6->setText(QString::number(data->pres[3]) + "MPa");
    ui->pres7->setText(QString::number(data->pres[5]) + "MPa");
    ui->pres8->setText(QString::number(data->pres[7]) + "MPa");

    ui->liquid1->setText(QString::number(data->liquid[0]) + "%");
    ui->liquid2->setText(QString::number(data->liquid[1]) + "%");

    ui->stove1Temp->setText(QString::number(data->PointTemp[0]) + "℃");
    ui->stove2Temp->setText(QString::number(data->PointTemp[1]) + "℃");
    ui->DisTemp1->setText(QString::number(data->PointTemp[2]) + "℃");
    ui->DisTemp2->setText(QString::number(data->PointTemp[3]) + "℃");
}

void diagram::autoRun(bool state)
{
    auto_Run = state;
}

void diagram::strToState(const QString& cmd)
{
    QStringList strlist;
    if(cmd.contains("---"))
       return ;    // 不处理功能标签

    if(cmd.contains("电磁阀") || cmd.contains("四通阀"))
    {
        strlist = cmd.split(" ");  //  以空格符分割
        uint8_t id = strlist.at(1).toInt();
        if(strlist.at(3) == "开")
        {
           swIconCtrl(id, true);
        }
        else if(strlist.at(3) == "关")
        {
           swIconCtrl(id, false);
        }
    }
    else if(cmd.contains("流量计"))
    {
//        if(cmd.contains("短接"))
//        {
//            strlist = cmd.split(" ");  //  以空格符分割
//            uint8_t id = strlist.at(1).toInt();
//            QString str = strlist.at(3);
//            if(str.contains("开启"))
//            {
//                flow_sw[id] = true;
//            }
//            else if(str.contains("关闭"))
//            {
//                flow_sw[id] = false;
//            }
//        }
        if(cmd.contains("流量设置"))
        {
            strlist = cmd.split(" ");  //  以空格符分割
            uint8_t id = strlist.at(1).toInt();
            QString str = strlist.at(3);
            flow_set[id] = str.toFloat();
        }
    }
    else if(cmd.contains("柱塞泵"))
    {
        strlist = cmd.split(" ");  //  以空格符分割
        uint8_t id = strlist.at(1).toInt();
        if(cmd.contains("流量"))    //格式：设备 1 流量设置为 x ml/min
        {
            QString str = strlist.at(3);
            pump_flow[id] = str.toDouble();
            if(id == 0)
            {
                ui->pumpflow1->setText(QString::number(pump_flow[id]) + "ml/min");
            }
            else if(id == 1)
            {
                ui->pumpflow2->setText(QString::number(pump_flow[id]) + "ml/min");
            }
        }
    }
    else if(cmd.contains("背压阀"))
    {
        strlist = cmd.split(" ");  //  以空格符分割
        uint8_t id = strlist.at(1).toInt();
        if(cmd.contains("流量"))    //格式：设备 1 流量设置为 x ml/min
        {
            QString str = strlist.at(3);
            pres_set[id] = str.toDouble();
        }
    }
}

void diagram::swIconCtrl(int id, bool state)
{
    if(swState[id] != state)  // 图标状态和命令不一致，才执行
    {
        if (id < 6)
            setSw(id, state);
    }
}

void diagram::updateHeatAndKeep(int idx, float tmp)
{
    const static QVector<QLabel*> sLabels = { ui->preHeatTemp, ui->preHeatTemp2, ui->canTemp, ui->canTemp2};
    if (idx < sLabels.size() && idx >= 0)
    {
        sLabels[idx]->setText(QString::number(tmp) + "℃");
    }
}
void diagram::updateTriEleValveStat(int idx, int pos, bool)
{
    if (idx != 0 && idx != 1)
        return;

    m_posValve3Way[idx] = pos;
}

void diagram::on_sw1_clicked()
{
    if( !check_run(QString("电磁阀1 设置为 %1").arg(swState[0]?"关":"开")) )
        return;

    setSw(0, !swState[0]);
}

void diagram::on_sw1_2_clicked()
{
    if (!check_run(QString("电磁阀3 设置为 %1").arg(swState[0] ? "关" : "开")))
        return;

    setSw(2, !swState[2]);
}

void diagram::on_sw1_3_clicked()
{
    if (!check_run(QString("电磁阀5 设置为 %1").arg(swState[0] ? "关" : "开")))
        return;

    setSw(4, !swState[4]);
}

void diagram::on_sw2_clicked()
{
    if( !check_run(QString("电磁阀2 设置为 %1").arg(swState[1]?"关":"开")) )
    {
        return;
    }

    setSw(1, !swState[1]);
}

void diagram::on_sw2_2_clicked()
{
    if (!check_run(QString("电磁开关阀4 设置为 %1").arg(swState[0] ? "关" : "开")))
        return;

    setSw(3, !swState[3]);
}

void diagram::on_sw2_3_clicked()
{
    if (!check_run(QString("电磁开关阀6 设置为 %1").arg(swState[0] ? "关" : "开")))
        return;

    setSw(5, false == swState[5]);
}

void diagram::on_flow1set_clicked()
{
    flowSet* flow = createFlowSet(QString(), 0, tr("流量计1"), flow_set[0]);
    connect(flow,&flowSet::flow_to_set,this,&diagram::flow_set_slot);
}

void diagram::on_flow2set_clicked()
{
    flowSet* flow = createFlowSet(QString(), 1, tr("流量计2"), flow_set[1]);
    connect(flow,&flowSet::flow_to_set,this,&diagram::flow_set_slot);
}

void diagram::on_pump1set_clicked()
{
    flowSet* flow = createFlowSet(QString(), 0, tr("柱塞泵1"), pump_flow[0]);
    connect(flow,&flowSet::flow_to_set,this,&diagram::flow_set_slot);
}

void diagram::on_pump2set_clicked()
{
    flowSet* flow = createFlowSet(QString(), 1, tr("柱塞泵2"), pump_flow[1]);
    connect(flow,&flowSet::flow_to_set,this,&diagram::flow_set_slot);
}

void diagram::on_pres1set_clicked()
{

    flowSet* flow = createFlowSet(tr("压力设置"), 0, tr("背压阀1"), pres_set[0], "MPa");
    connect(flow,&flowSet::flow_to_set,this,&diagram::flow_set_slot);
}

void diagram::on_pres2set_clicked()
{
    flowSet *flow = createFlowSet(tr("压力设置"), 1, tr("背压阀2"), pres_set[1], "MPa");
    connect(flow, &flowSet::flow_to_set, this, &diagram::flow_set_slot);
}

void diagram::on_pres1_min_clicked()
{
	flowSet *flow = createFlowSet(tr("压力设置"), 0, tr("减压阀1"), pres_set[2], "MPa");
	connect(flow, &flowSet::flow_to_set, this, &diagram::flow_set_slot);
}

void diagram::on_pres2_min_clicked()
{
	flowSet *flow = createFlowSet(tr("压力设置"), 1, tr("减压阀2"), pres_set[3], "MPa");
	connect(flow, &flowSet::flow_to_set, this, &diagram::flow_set_slot);
}

void diagram::flow_set_slot(QString str)
{
    if(str.contains("流量计"))
    {
       QStringList list = str.split(" ");
       uint8_t id = list.at(1).toInt();
       flow_set[id] = QString(list.at(3)).toDouble();
    }
    else if(str.contains("柱塞泵"))
    {
       QStringList list = str.split(" ");
       uint8_t id = list.at(1).toInt();
       pump_flow[id] = QString(list.at(3)).toDouble();
       if(id == 0)
       {
           ui->pumpflow1->setText(QString::number(pump_flow[0]) + "ml/min");
       }
       else if(id == 1)
       {
           ui->pumpflow2->setText(QString::number(pump_flow[1]) + "ml/min");
       }

    }
    else if(str.contains("背压阀"))
    {
       QStringList list = str.split(" ");
       uint8_t id = list.at(1).toInt();
       pres_set[id] = QString(list.at(3)).toDouble();
    }
    else if(str.contains("减压阀"))
    {
       QStringList list = str.split(" ");
       uint8_t id = list.at(1).toInt();
       pres_set[id+2] = QString(list.at(3)).toDouble();
    }

    cmdTorun(str);
}

bool diagram::check_run(const QString& str)
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
    return true;
}

void diagram::initSlots()
{
    connect(ui->btn_valve3way1, &QPushButton::clicked, this, [=] {
        flowSet* flow = createFlowSet(tr("电动3通阀设置"), 0, tr("电动3通阀1"), int(m_posValve3Way[0]));
        connect(flow, &flowSet::flow_to_set, this, [=](const QString& str) {
            auto stLs = str.split(" ", QString::SkipEmptyParts);
            auto idx = stLs.indexOf(tr("通向"));
            if (idx >= 0 && idx + 1 < stLs.count())
                m_posValve3Way[0] = stLs.at(idx + 1).toInt();

            cmdTorun(str);
        });
    });

    connect(ui->btn_valve3way2, &QPushButton::clicked, this, [=] {
        flowSet* flow = createFlowSet(tr("电动3通阀设置"), 1, tr("电动3通阀2"), int(m_posValve3Way[1]));
        connect(flow, &flowSet::flow_to_set, this, [=](const QString& str) {
            auto stLs = str.split(" ", QString::SkipEmptyParts);
            auto idx = stLs.indexOf(tr("通向"));
            if (idx >= 0 && idx + 1 < stLs.count())
                m_posValve3Way[1] = stLs.at(idx + 1).toInt();

            cmdTorun(str);
        });
    });
}

flowSet* diagram::createFlowSet(const QString &title, int id, const QString& name, const QVariant& v, const QString& unit)
{
    flowSet* flow = new flowSet;
    if (!name.isEmpty())
        flow->set_flow_name(name, id);
    flow->set_flow_val(v);
    if (!title.isEmpty())
        flow->set_title(title);
    if (!unit.isEmpty())
        flow->set_unit(unit);

    auto desktop = QApplication::primaryScreen();
    QRect screen = desktop->geometry();
    int x = (screen.width() - flow->width()) / 2;
    int y = (screen.height() - flow->height()) / 2;
    flow->move(x, y);

    flow->raise();
    flow->show();
    return flow;
}

void diagram::setSw(int idx, bool b)
{
    static QVector<QPushButton*> sws = { ui->sw1,ui->sw2,ui->sw1_2,ui->sw2_2,ui->sw1_3,ui->sw2_3 };
    if (idx < 0 || idx >= sws.size())
        return;

    if (b)
    {
        sws[idx]->setIcon(QIcon(":/Diagram/image/sw1_on.png"));
        swState[idx] = true;
        emit cmdTorun(tr("电磁阀 %1 设置为 开").arg(idx));
    }
    else
    {
        sws[idx]->setIcon(QIcon(":/Diagram/image/sw1_off.png"));
        swState[idx] = false;
        emit cmdTorun(tr("电磁阀 %1 设置为 关").arg(idx));
    }
}