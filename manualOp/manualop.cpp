#include "manualop.h"
#include "ui_manualop.h"
#pragma execution_character_set("utf-8")

manualOp::manualOp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::manualOp)
{
    ui->setupUi(this);

    connect(ui->Diagram, &diagram::cmdTorun,this,&manualOp::readyTorun);
}

manualOp::~manualOp()
{
    delete ui;
}

void manualOp::updateInfo(ReceiveData *data)
{
    ui->Diagram->updateInfo(data);
}
diagram* manualOp::getDiagram()
{
    return ui->Diagram;
}

void manualOp::strToState(const QString& cmd)
{
    ui->Diagram->strToState(cmd);
}

void manualOp::autoRun(bool state)
{
    ui->Diagram->autoRun(state);
}

void manualOp::readyTorun(const QString& str)
{
    emit cmdTorun(str);
}
