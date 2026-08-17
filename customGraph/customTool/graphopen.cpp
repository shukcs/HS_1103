#include "graphopen.h"
#include "ui_graphopen.h"
#pragma execution_character_set("utf-8")

graphOpen::graphOpen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphOpen)
{
    ui->setupUi(this);

    rescan_file();
}

graphOpen::~graphOpen()
{
    delete ui;
}

void graphOpen::rescan_file()
{
    QString path = QCoreApplication::applicationDirPath()+"/graph";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
       dir.mkdir(path);

    QStringList filter = {"*.csv"};
    dir.setNameFilters(filter);
    ui->file_list->clear();
    for (auto &itr : dir.entryInfoList(filter))
    {
        ui->file_list->addItem(itr.fileName());
    }
}


void graphOpen::on_file_open_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/graph/";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }

    if(ui->file_list->currentRow() < 0)
    {
       return;
    }

    QString name = ui->file_list->currentItem()->text();
    if(name == "")
    {
        return;
    }

    file_to_open(path + name);

    this->deleteLater();
}

void graphOpen::on_back_clicked()
{
     this->deleteLater();
}

void graphOpen::on_file_del_clicked()
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
               QFile::remove(path);
               int row = ui->file_list->currentRow();
               QListWidgetItem *item = ui->file_list->takeItem(row);
               delete item;
            }
        }
    }
}

void graphOpen::on_file_clear_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/graph";
    QDir dir(path);
    dir.removeRecursively();
    ui->file_list->clear();
}
