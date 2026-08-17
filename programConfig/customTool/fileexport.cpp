#include "fileexport.h"
#include "ui_fileexport.h"

fileExport::fileExport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileExport)
{
    ui->setupUi(this);
}

fileExport::~fileExport()
{
    delete ui;
}

void fileExport::on_file_export_clicked()
{

}

void fileExport::on_file_delete_clicked()
{

}

void fileExport::on_file_clear_clicked()
{

}

void fileExport::on_back_clicked()
{
     this->deleteLater();
}
