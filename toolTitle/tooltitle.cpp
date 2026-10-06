#include "tooltitle.h"
#include "HsApplication.h"
#include "desktop.h"
#include "deviceOp/deviceop.h"
#include "ui_tooltitle.h"
#pragma execution_character_set("utf-8")

toolTitle::toolTitle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::toolTitle)
{
    ui->setupUi(this);

    connect(ui->logo,SIGNAL(logo_clicked()),this,SLOT(logoBtn_slot()));
}

toolTitle::~toolTitle()
{
    delete ui;
}

void toolTitle::logoBtn_slot()
{
    auto des = qobject_cast<deskTop*>(hsApp->mainWidget());
	des->Scene5Show();
    des->getDeviceOp()->tempAdjustEnable();
}

