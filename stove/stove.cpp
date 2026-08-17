#include "stove.h"
#include "ui_stove.h"
#pragma execution_character_set("utf-8")

stove::stove(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::stove)
{
    ui->setupUi(this);
    autoRun = false;

    QFile styleFile;
    styleFile.setFileName(":/stove/stove.qss");
    if (styleFile.exists() ) {
        styleFile.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
    }

    data = new QList<int>;

    ui->tempRise->setIconSize(QSize(35,35));
    ui->tempFall->setIconSize(QSize(35,35));
    ui->dataSave->setIconSize(QSize(35,35));
    ui->tempRise->setIcon(QIcon(":/stove/image/up.png"));
    ui->tempFall->setIcon(QIcon(":/stove/image/down.png"));
    ui->dataSave->setIcon(QIcon(":/stove/image/save.png"));

    connect(ui->stoveSeleBox, static_cast<void(QComboBox::*)(const QString &name)>(&QComboBox::currentTextChanged), this, [=](const QString &name){
        ui->table->saveTableTemp();
        setTitle(name);
        Table_init();
        emit stove_change(ui->stoveSeleBox->currentIndex());
    });

    connect(ui->table,&StoveTable::stove_refresh,this,[=](int row,int col,float val){
        emit stove_refresh(ui->stoveSeleBox->currentIndex(),row,col,val);
    });
}

stove::~stove()
{
    delete ui;
}

void stove::setTitle(QString str)
{
    ui->table->setObjectName(str);
}

QString stove::getTitleName()
{
    return ui->stoveSeleBox->currentText();
}

void stove::setTime(QString str)
{
    ui->time->setText(str);
}

void stove::setTemp(QString str)
{
    ui->temp->setText(str);
}

void stove::setStep(QString str)
{
    ui->step->setText(str);
}

void stove::autoState(bool run)
{
    autoRun = run;
}

void stove::manStoveRise(int stove)
{
    ui->stoveSeleBox->setCurrentIndex(stove);
    on_tempRise_clicked();
}

void stove::manStoveFall(int stove)
{
    ui->stoveSeleBox->setCurrentIndex(stove);
    on_tempFall_clicked();
}

//void stove::setTableHeader(QStringList header)
//{
//    ui->table->setTableHeader(header);
//}

void stove::setTableFormat(int col, int row)
{
    ui->table->setTableFormat(col,row);
}

void stove::Table_init()
{
    ui->table->Table_init();
}

int stove::get_col()
{
    return ui->table->get_col();
}

int stove::get_row()
{
    return ui->table->get_row();
}

void stove::setTabelTemp(int *temp, int *time)
{
    ui->table->setTabelTemp(temp,time);
}

void stove::on_tempRise_clicked()
{
    int i;
    int num = ui->table->get_row();  // 获取程序段数
    num = 2 * num;  // 时间+温度
    QString cmd = "程序升温 "+QString::number(ui->stoveSeleBox->currentIndex())+" ";

    if(!check_run(cmd))
    {
        return;
    }

    ui->table->getTableTemp(data);
    for(i=0;i<num;i++)
    {
       cmd = cmd + QString::number(data->at(i))+ " ";
    }
    emit StoveTempRise(cmd);
    MyMessageBox msg(MyMessageBox::Success, "提示", ui->stoveSeleBox->currentText()+" 开始运行", MyMessageBox::Ok,this);
    msg.exec();
}

void stove::on_tempFall_clicked()
{
    QString cmd = ui->stoveSeleBox->currentText()+" "+QString::number(ui->stoveSeleBox->currentIndex())+" 降温";

    if(!check_run(cmd))
    {
        return;
    }

    emit StoveTempFalling(cmd);
    MyMessageBox msg(MyMessageBox::Success, "提示", ui->stoveSeleBox->currentText()+" 已停止", MyMessageBox::Ok,this);
    msg.exec();
}

void stove::on_dataSave_clicked()
{
    ui->table->saveTableTemp();
    MyMessageBox msg(MyMessageBox::Success, "提示", ui->stoveSeleBox->currentText()+" 保存成功", MyMessageBox::Ok,this);
    msg.exec();
}

bool stove::check_run(QString str)
{
    if(autoRun)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("确认");

        // 先清空所有样式
        msgBox.setStyleSheet("");

        // 重新设置完整样式
        msgBox.setStyleSheet(R"(
            QMessageBox, QMessageBox * {
                background-color: white;
                color: black;
                font-size: 14px;
            }

            QPushButton {
                background-color: #785093;
                color: black;
                border: 1px solid #cccccc;
                border-radius: 4px;
                padding: 6px 12px;
                min-width: 80px;
            }

            QPushButton:hover {
                background-color: #0080ff;
                color: white;
            }

            QPushButton#qt_msgbox_yes {
                background-color: #00ffff;
            }

            QPushButton#qt_msgbox_no {
                background-color: #785093;
            }
        )");

        msgBox.setText(QString("是否继续执行命令：\n%1").arg(str));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if(msgBox.exec() != QMessageBox::Yes)
        {
            return false;
        }
    }
    return true;
}
