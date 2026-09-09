#include "desktop.h"
#include <QApplication>
#include <QSharedMemory>
#include "common/mymessagebox.h"
#pragma execution_character_set("utf-8")


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    static QSharedMemory *runApp = new QSharedMemory(qApp->applicationName().toStdString().c_str());//创建“App名称”的共享内存块

    if(!runApp->create(1))//创建失败，表示已经在运行，
    {
        MyMessageBox msg(MyMessageBox::Success, "提示", "程序正在运行", MyMessageBox::Ok);
        msg.exec();

        qApp->quit();
        return -1;
    }

    deskTop w;
    w.showMaximized();

    return a.exec();
}
