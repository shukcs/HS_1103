#include "filesave.h"
#include "ui_filesave.h"
#pragma execution_character_set("utf-8")

fileSave::fileSave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileSave)
{
    ui->setupUi(this);
}

fileSave::~fileSave()
{
    delete ui;
}

void fileSave::on_file_save_clicked()
{
    QString path = QCoreApplication::applicationDirPath()+"/config";
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

    QString str = path + "/" + ui->file_name->text() + ".txt";
    int cnt = listWidget->count();
    if(cnt == 0)
    {
        MyMessageBox msg(MyMessageBox::Success, "提示", "请添加项目", MyMessageBox::Ok,this);
        msg.exec();
            return ;
    }

    QString text;
    int num = 0;
    for(int i=0;i<cnt;i++)
    {
        text = listWidget->item(i)->text();
        if(text.contains("---"))  // 检测含有项目类别
        {
            num++;
        }
    }
    if(num > 0)
    {
        // qDebug()<<"OK";
    }
    else
    {
        MyMessageBox msg(MyMessageBox::Success, "提示", "请添加标签", MyMessageBox::Ok,this);
        msg.exec();
        return;
    }


    QFile file(str);
    file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate );
    QTextStream out(&file);
//        QString text;
    for(int i=0;i<cnt;i++)
    {
        text = listWidget->item(i)->text() + "\n";
        out<<tr(text.toStdString().c_str());
    }
    file.close();

    deleteLater();
}

void fileSave::on_back_clicked()
{
     this->deleteLater();
}
