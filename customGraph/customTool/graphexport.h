#ifndef GRAPHEXPORT_H
#define GRAPHEXPORT_H

#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QListWidget>
#include <QTextStream>
#include <QDebug>

#include "common/mymessageBox.h"

namespace Ui {
class graphExport;
}

class graphExport : public QWidget
{
    Q_OBJECT

public:
    explicit graphExport(QWidget *parent = 0);
    ~graphExport();

    void rescan_file();

    bool u_disk_check();

private slots:
    void on_file_export_clicked();

    void on_back_clicked();

private:
    Ui::graphExport *ui;
};

#endif // GRAPHEXPORT_H
