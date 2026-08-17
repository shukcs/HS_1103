#include "statebar.h"
#include "ui_statebar.h"
#pragma execution_character_set("utf-8")


stateBar::stateBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::stateBar)
{
    ui->setupUi(this);

    ui->graphBtn->setIcon(QIcon(":/stateBar/image/graph.png"));
    ui->graphBtn->setIconSize(QSize(30,30));

    ui->manualBtn->setIcon(QIcon(":/stateBar/image/manual.png"));
    ui->manualBtn->setIconSize(QSize(30,30));

    ui->autoBtn->setIcon(QIcon(":/stateBar/image/auto.png"));
    ui->autoBtn->setIconSize(QSize(30,30));

    ui->collectorBtn->setIcon(QIcon(":/stateBar/image/water.png"));
    ui->collectorBtn->setIconSize(QSize(30,30));
    ui->collectorBtn->setVisible(false);

    ui->connectState->setIcon(QIcon(":/stateBar/image/disconnected.png"));
    ui->connectState->setIconSize(QSize(30, 30));

    ui->btn_store->setIconSize(QSize(30, 30));
    ui->btn_store->setIcon(QIcon(":/image/feeder.png"));

    ui->sampleTime->setToolTip("ms");
    ui->sampleTime->addItem("200");
    ui->sampleTime->addItem("300");
    ui->sampleTime->addItem("400");
    ui->sampleTime->addItem("500");
    ui->sampleTime->addItem("600");
    ui->sampleTime->addItem("700");
    ui->sampleTime->addItem("800");
    ui->sampleTime->addItem("900");
    ui->sampleTime->addItem("1000");
    ui->sampleTime->setCurrentIndex(8);  // 默认选择1000ms

    QString path = QCoreApplication::applicationDirPath()+"/user";
    QDir dir(path);
    if(!dir.exists())   //  检查目录是否存在
    {
       dir.mkdir(path);
    }
    path = path + "/port";
    file.setFileName(path);    //  读取文件中的数据
    if(file.exists())   //  如果文件存在
    {
        file.open(QFile::ReadWrite);
        QByteArray data = file.readAll();
        QString str = QString::fromUtf8(data);
        if(str.contains("COM"))
        {
          port.name = str;
        }
        else
        {
#ifdef Q_OS_WIN
          port.name = "COM7";  // 默认使用COM7
#else
          port.name = "ttySTM1";
#endif

        }
        file.close();
    }
    else {
        file.open(QFile::ReadWrite);
#ifdef Q_OS_WIN
          port.name = "COM7";  // 默认使用COM7
#else
          port.name = "ttySTM1";
#endif
        QTextStream text(&file);
        text << port.name;
        file.close();
    }
    connect(ui->btn_store, &QPushButton::clicked, this, [=] { emit storeBtn_clicked(); });
}

stateBar::~stateBar()
{
    delete ui;
}

void stateBar::setConnectionState(bool state)
{
    if(state)
    {
      ui->connectState->setIcon(QIcon(":/stateBar/image/connected.png"));
      ui->connectState->setText("已连接");
    }
    else
    {
      ui->connectState->setIcon(QIcon(":/stateBar/image/disconnected.png"));
      ui->connectState->setText("未连接");
    }
}

void stateBar::on_sampleTime_currentTextChanged(const QString &arg1)
{
     emit sampleTimeChanged(arg1.toInt());
}

void stateBar::on_connectState_clicked()
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("端口");
    dlg->setFixedSize(250,100);
    QFile Stylefile;
    Stylefile.setFileName(":/stateBar/stateBar.qss");
    if (Stylefile.exists() ) {
        Stylefile.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(Stylefile.readAll());
        dlg->setStyleSheet(styleSheet);
        Stylefile.close();
    }

    QHBoxLayout *layout = new QHBoxLayout;
    dlg->setLayout(layout);

    QLabel *passwd = new QLabel;
    passwd->setText("默认端口");
    layout->addWidget(passwd);
    layout->addStretch();

    QComboBox *portList = new QComboBox;
    portList->setView(new QListView);  //  必须加入，否则部分样式不生效
    layout->addWidget(portList);
    layout->addStretch();

    QList<QSerialPortInfo> com_info = QSerialPortInfo::availablePorts();
    portList->addItem(port.name); //  将保存的端口显示在第一个
    int count = com_info.count();
    for(int i=0;i<count;i++)
    {
        if(com_info.at(i).portName() != port.name)
        {
          portList->addItem(com_info.at(i).portName());
        }
    }
    QPushButton *btn = new QPushButton;
    btn->setText("保存");
    btn->setFixedSize(40,20);
    layout->addWidget(btn);

    connect(btn,&QPushButton::clicked,this,[&]{
        if(portList->currentText().contains("COM"))    // 按下保存，保存端口号
        {
            file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate );
            port.name = portList->currentText();
            QTextStream text(&file);
            text << port.name;
            file.close();
            emit portChanged(port.name);
            dlg->close();
        }
    });

    dlg->exec();
}

void stateBar::on_autoBtn_clicked()
{
    emit autoBtn_clicked();
}

void stateBar::on_manualBtn_clicked()
{
    emit manualBtn_clicked();
}

void stateBar::on_collectorBtn_clicked()
{
    emit collectorBtn_clicked();
}

void stateBar::on_graphBtn_clicked()
{
    emit graphBtn_clicked();
}
