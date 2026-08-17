#ifndef GRAPHSAVE_H
#define GRAPHSAVE_H

#include <QWidget>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>

namespace Ui {
class graphSave;
}

class graphSave : public QWidget
{
    Q_OBJECT

public:
    explicit graphSave(QWidget *parent = 0);
    ~graphSave();

signals:
    void file_saved(QString name);

private slots:
    void on_file_save_clicked();

    void on_back_clicked();

private:
    Ui::graphSave *ui;
};

#endif // GRAPHSAVE_H
