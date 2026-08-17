#include "graphexport.h"
#include "ui_graphexport.h"
#pragma execution_character_set("utf-8")

graphExport::graphExport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphExport)
{
    ui->setupUi(this);

    rescan_file();
}

graphExport::~graphExport()
{
    delete ui;
}


void graphExport::rescan_file()
{
    QString path = QCoreApplication::applicationDirPath()+"/graph";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
       dir.mkdir(path);

    QStringList filter = { "*.csv" };
    dir.setNameFilters(filter);
    ui->file_list->clear();
    for (auto &itr : dir.entryInfoList(filter))
    {
        ui->file_list->addItem(itr.fileName());
    }
}

bool graphExport::u_disk_check()
{
    QDir dir;
    if(dir.exists("/run/media/sda1"))
    {
       return true;
    }
    else
    {
        return false;
    }
    return false;
}

void graphExport::on_file_export_clicked()
{
    if(ui->file_list->count() > 0)
    {
        int row = ui->file_list->currentRow();
        if(row >= 0)
        {
            QString path = QCoreApplication::applicationDirPath()+"/graph/"+ ui->file_list->currentItem()->text();
            QFileInfo FileInfo(path);
            if(FileInfo.isFile())
            {
               int ret = 0;
               if(u_disk_check())
               {
                  QString str = "cp "+ path  + " /run/media/sda1/";
                  ret = system(str.toStdString().c_str()) ;
                  if(ret == 0)
                  {
                    str = "sync";
                    system(str.toStdString().c_str()) ;
                    MyMessageBox msg(MyMessageBox::Critical, "提示", "导出成功", MyMessageBox::Ok,this);
                    msg.exec();
                  }
                  else
                  {
                    MyMessageBox msg(MyMessageBox::Critical, "提示", "导出失败", MyMessageBox::Ok,this);
                    msg.exec();
                  }
               }
               else
               {
                   MyMessageBox msg(MyMessageBox::Critical, "提示", "请插入U盘", MyMessageBox::Ok,this);
                   msg.exec();
               }

            }
        }
    }
}

void graphExport::on_back_clicked()
{
    this->deleteLater();
}
