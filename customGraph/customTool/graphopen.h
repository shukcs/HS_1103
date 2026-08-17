#ifndef GRAPHOPEN_H
#define GRAPHOPEN_H

#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QListWidget>
#include <QTextStream>
#include <QDebug>

namespace Ui {
class graphOpen;
}

class graphOpen : public QWidget
{
    Q_OBJECT

public:
    explicit graphOpen(QWidget *parent = 0);
    ~graphOpen();

    void rescan_file();

signals:
    void file_to_open(QString name);

private slots:
    void on_file_open_clicked();

    void on_back_clicked();

    void on_file_del_clicked();

    void on_file_clear_clicked();


private:
    Ui::graphOpen *ui;
};

#endif // GRAPHOPEN_H
