#ifndef FILEEXPORT_H
#define FILEEXPORT_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class fileExport;
}

class fileExport : public QWidget
{
    Q_OBJECT

public:
    explicit fileExport(QWidget *parent = 0);
    ~fileExport();

private slots:
    void on_file_export_clicked();

    void on_file_delete_clicked();

    void on_file_clear_clicked();

    void on_back_clicked();

private:
    Ui::fileExport *ui;
};

#endif // FILEEXPORT_H
