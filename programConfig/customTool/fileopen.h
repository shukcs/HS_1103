#ifndef FILEOPEN_H
#define FILEOPEN_H

#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QListWidget>
#include <QTextStream>

namespace Ui {
class fileOpen;
}

class fileOpen : public QWidget
{
    Q_OBJECT

public:
    explicit fileOpen(QWidget *parent = 0);
    ~fileOpen();

    void rescan_file();

    QListWidget *listWidget;  // 保存当前list的指针

    bool appendFlag = false;

    void setTitle(QString name);

private slots:
    void on_file_open_clicked();

    void on_back_clicked();

    void on_file_del_clicked();

    void on_file_clear_clicked();

private:
    Ui::fileOpen *ui;
};

#endif // FILEOPEN_H
