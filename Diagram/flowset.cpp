#include "flowset.h"
#include "ui_flowset.h"
#pragma execution_character_set("utf-8")

flowSet::flowSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::flowSet)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    ui->setupUi(this);
}

flowSet::~flowSet()
{
    delete ui;
}

void flowSet::set_title(QString name)
{
    ui->label_title->setText(name);
}

void flowSet::set_flow_name(QString name,uint8_t id)
{
    dev_id = id;
    ui->label_flow_name->setText(name);
}

void flowSet::set_unit(QString name)
{
    ui->label_unit->setText(name);
}

void flowSet::set_flow_val(const QVariant &val)
{
    if (val.type() == QVariant::Double)
    {
        ui->flow->setText(QString::number(val.toDouble()));
    }
    else if (val.canConvert<int>())
    {
        ui->comboBox->setCurrentIndex(val.toInt());
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void flowSet::on_ok_clicked()
{
    if (1 == ui->stackedWidget->currentIndex())
    {
        emit flow_to_set(tr("电动三通阀 通道 %1 通向 %2").arg(dev_id).arg(ui->comboBox->currentIndex()));
    }
    else
    {
        if(ui->flow->text().isEmpty())
           return;

        QString str = ui->label_flow_name->text()+" "+QString::number(dev_id) + " "+ui->label_title->text()+ " " + ui->flow->text() + " " + ui->label_unit->text();
        emit flow_to_set(str);
    }

    deleteLater();
}

void flowSet::on_back_clicked()
{
     deleteLater();
}
