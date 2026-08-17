#include "fileopen.h"
#include "ui_fileopen.h"
#pragma execution_character_set("utf-8")

fileOpen::fileOpen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileOpen)
{
    ui->setupUi(this);
    rescan_file();
}

fileOpen::~fileOpen()
{
    delete ui;
}


void fileOpen::rescan_file()
{
    QString path = QCoreApplication::applicationDirPath()+"/config";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
       dir.mkdir(path);

    QStringList filter = {"*.txt"};
    dir.setNameFilters(filter);
    ui->file_list->clear();
    for (auto &fileInfo : dir.entryInfoList(filter))
    {
        ui->file_list->addItem(fileInfo.fileName());
    }
}

void fileOpen::setTitle(QString name)
{
     ui->label->setText(name);
}

//    QString path = QCoreApplication::applicationDirPath()+"/config";
//    QDir dir(path);
//    if(!dir.exists())   //  检查目录是否存在
//    {
//       dir.mkdir(path);
//    }
//    QString str = QFileDialog::getOpenFileName(this,"open",path,"(*.txt)");  // 打开文件
//    if(str == "")
//    {
//       return;    //  “取消” 退出
//    }
//    QFile readFile(str);
//    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
//        return ;

//    ui->listWidget->clear();  //  清空当前列表

//    QTextStream stream(&readFile);    //  读取文件
//    QString line;
//    while (!stream.atEnd())
//    {
//        line = stream.readLine();   //  逐行读取
//        if(line == NULL)
//        {
//           readFile.close();
//           return ;
//        }
//        ui->listWidget->addItem(line);
//    }
//    readFile.close();

void fileOpen::on_file_open_clicked()
{
      QString path = QCoreApplication::applicationDirPath()+"/config/";
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

      QFile readFile(path + name);
      if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
          return ;
      if(appendFlag == false) // 不是追加项目
      {
        listWidget->clear();  //  清空当前列表
      }

      QTextStream stream(&readFile);    //  读取文件
      QString line;
      while (!stream.atEnd())
      {
          line = stream.readLine();   //  逐行读取
          if(line.isEmpty())
          {
             readFile.close();
             return ;
          }
          listWidget->addItem(line);
      }
      readFile.close();

      this->deleteLater();
}

void fileOpen::on_back_clicked()
{
     this->deleteLater();
}

void fileOpen::on_file_del_clicked()
{
    if(ui->file_list->count() > 0)
    {
        int row = ui->file_list->currentRow();
        if(row >= 0)
        {
            QString path = QCoreApplication::applicationDirPath()+"/config/"+ ui->file_list->currentItem()->text();
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

void fileOpen::on_file_clear_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/config";
    QDir dir(path);
    dir.removeRecursively();
    ui->file_list->clear();
}
