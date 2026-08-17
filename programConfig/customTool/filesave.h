#ifndef FILESAVE_H
#define FILESAVE_H

#include <QWidget>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include "common/mymessageBox.h"

namespace Ui {
class fileSave;
}

class fileSave : public QWidget
{
    Q_OBJECT

public:
    explicit fileSave(QWidget *parent = 0);
    ~fileSave();

    QListWidget *listWidget;

private slots:
    void on_file_save_clicked();

    void on_back_clicked();

private:
    Ui::fileSave *ui;
};

#endif // FILESAVE_H
