#include "graphsave.h"
#include "ui_graphsave.h"
#include "common/mymessageBox.h"
#pragma execution_character_set("utf-8")

graphSave::graphSave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphSave)
{
    ui->setupUi(this);
}

graphSave::~graphSave()
{
    delete ui;
}

void graphSave::on_file_save_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/graph";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }

    if(ui->file_name->text().isEmpty())
    {
        MyMessageBox msg(MyMessageBox::Success, "提示", "请输入文件名", MyMessageBox::Ok,this);
        msg.exec();
        return ;
    }

    QString str = path + "/" + ui->file_name->text() + ".csv";

    file_saved(str);

    this->deleteLater();
}

void graphSave::on_back_clicked()
{
     this->deleteLater();
}
