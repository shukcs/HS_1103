#include "collectorop.h"
#include <qevent.h>
#include <QTimer>
#include "ui_collectorop.h"
#pragma execution_character_set("utf-8")

collectorOp::collectorOp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::collectorOp)
{
    ui->setupUi(this);

    labelInit();

    connect(this, &collectorOp::selfcmdTorun,this, &collectorOp::check_run);

    /******************************************************
    * comboBox只做展示用，需要拦截鼠标事件，还有没有焦点
    *******************************************************/
    mask = new QLabel(ui->sa_widget);
    mask->setStyleSheet("background-color: rgba(0,0,0,0);"); // 完全透明
    auto rc = QRect(QPoint(0,0), ui->sa_widget->sizeHint());
    mask->setGeometry(rc);  //widget_5绝对位置
    mask->raise(); // 提升到最上层
    mask2 = new QLabel(ui->stateBox);
    mask2->setFocusPolicy(Qt::ClickFocus); //蒙版拦截stateBox焦点
    mask2->setStyleSheet("background-color: rgba(0,0,0,0);"); // 完全透明
    rc = QRect(QPoint(0, 0), ui->stateBox->sizeHint());
    mask2->setGeometry(rc);  //widget_5绝对位置
    mask2->raise(); // 提升到最上层
    ui->sa_widget->installEventFilter(this);  //自动调整大小
    ui->stateBox->installEventFilter(this);   //自动调整大小
    mask->installEventFilter(this);//拦截遮罩层的鼠标事件
    mask2->installEventFilter(this);

    QString comstyle = R"(
            QComboBox {
                background-color: transparent;
                color: black;
                border: 1px solid #c0c0c0;
                border-radius: 6px;
                padding: 4px 8px;
                font-size: 18px;
                font-weight: bold;
                min-height: 28px;
            }

            QComboBox:hover {
                border: 1px solid #a0a0a0;
            }

            QComboBox:focus {
                border: 1px solid #785093;
                background-color: #f8f8f8;
            }

            QComboBox::drop-down {
                border: none;
                background: transparent;
                width: 28px;
            }

            QComboBox::down-arrow {
                border-left: 5px solid transparent;
                border-right: 5px solid transparent;
                border-top: 8px solid #606060;
                width: 0;
                height: 0;
                margin-right: 8px;
            }

            QComboBox QAbstractItemView {
                background-color: white;
                border: 1px solid #c0c0c0;
                border-radius: 4px;
                selection-background-color: #785093;
                selection-color: white;
                outline: none;
            }

            QComboBox QAbstractItemView::item {
                padding: 6px 8px;
                font-size: 18px;
                color: #333333;
                min-height: 28px;
            }

            QComboBox QAbstractItemView::item:hover {
                background-color: #f0f0f0;
            }

            QComboBox QAbstractItemView::item:selected {
                background-color: #785093;
                color: white;
            }
        )";
    ui->stateBox->setStyleSheet(comstyle);

    QString comstyle_sub = R"(
        QComboBox {
            background-color: white;
            border: 1px solid #e0e0e0;
            padding: 2px 16px 2px 6px;
            font-size: 14px;
            font-weight: bold;
            color: #333;
        }

        QComboBox:hover {
            border-color: #2196f3;
            background-color: #fafafa;
        }

        QComboBox:focus {
            border-color: #2196f3;
            background-color: white;
        }

        QComboBox::drop-down {
            border: none;
            width: 30px;
            subcontrol-position: right center;
        }

        QComboBox::down-arrow {
            width: 0;
            height: 0;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 5px solid #666;
            margin-right: 8px;
        }

        QComboBox::down-arrow:hover {
            border-top-color: #2196f3;
        }

        QComboBox QAbstractItemView {
            background-color: white;
            border: 1px solid #e0e0e0;
            border-radius: 6px;
            selection-background-color: #e3f2fd;
            selection-color: #2196f3;
            padding: 4px;
            outline: none;
        }

        QComboBox QAbstractItemView::item {
            padding: 4px 6px;
            border-radius: 4px;
        }

        QComboBox QAbstractItemView::item:selected {
            background-color: #e3f2fd;
        }
    )";

    for (int i = 1; i <= 32; ++i) {
        QString objName = QString("comboBox_%1").arg(i);
        QComboBox* combox = ui->sa_widget->findChild<QComboBox*>(objName);
        if (combox) {
            combox->setStyleSheet(comstyle_sub);
            combox->addItems({
                "无采样瓶(初始化状态)",
                "检测到采样瓶",
                "采集到样品完成",
                "样品抽取N毫升完成",
                "稀释液注入N毫升完成"
            });
            connect(combox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this, &collectorOp::LabelUpdate);
        }
    }
    ui->comboBox->setStyleSheet(comstyle_sub);
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this, &collectorOp::SampleUpdate);

    QIntValidator* validator_int = new QIntValidator(0, 32, this);
    ui->lineEdit_3->setValidator(validator_int);

    QRegularExpression rx("^(0\\.[1-9]|1\\.\\d|2\\.0)$");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(rx, this);
    ui->lineEdit_5->setValidator(validator);
    ui->lineEdit_6->setValidator(validator);
}

collectorOp::~collectorOp()
{
    delete ui;
}

bool collectorOp::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj==mask || obj==mask2) && dynamic_cast<QMouseEvent*>(event)!=nullptr)
    {
        return true; // 拦截点击
    }
    if (obj == ui->sa_widget && event->type() == QEvent::Resize) {
        auto rc = QRect(QPoint(0, 0), ((QResizeEvent*)event)->size());
        mask->setGeometry(rc);  //widget_5绝对位置
        mask->raise(); // 提升到最上层
    }
    else if (obj == ui->stateBox && event->type() == QEvent::Resize)
    {
        auto rc = QRect(QPoint(0, 0), ((QResizeEvent*)event)->size());
        mask2->setGeometry(rc);  //widget_5绝对位置
        mask2->raise(); // 提升到最上层
    }
    return QWidget::eventFilter(obj, event);
}

void collectorOp::labelInit(void)
{
    QString labelstyle = R"(
            QLabel {
                color: red;
                font-size: 18px;
            }
        )";
    for (int i = 40; i <= 71; ++i) {
        QString objName = QString("label_%1").arg(i);
        QLabel* label = this->findChild<QLabel*>(objName);
        if (label) {
            label->setStyleSheet(labelstyle);
            label->setText("--");
        }
    }
}

void collectorOp::autoRun(bool state)
{
    auto_Run = state;
}

void collectorOp::updateCmd(const QString& str)
{
    QStringList strlist;

    strlist = str.split(" ");  //  以空格符分割
    //int num = strlist.at(1).toInt();
    QString state = strlist.at(2);
    if(state == "开始采样(采样模式)")
    {
        ui->comboBox->setCurrentIndex(0);
        ui->lineEdit_3->setText(strlist.at(3));
        ui->lineEdit_1->setText(strlist.at(4));
        ui->lineEdit_2->setText(strlist.at(5));
    }
    else if(state == "开始采样(采样+稀释)")
    {
        ui->comboBox->setCurrentIndex(1);
        ui->lineEdit_3->setText(strlist.at(3));
        ui->lineEdit_1->setText(strlist.at(4));
        ui->lineEdit_2->setText(strlist.at(5));
    }
    else if((state == "暂停采样"))
    {
        ui->comboBox->setCurrentIndex(3);
    }
    else if((state == "继续采样"))
    {
        ui->comboBox->setCurrentIndex(4);
    }
//    else if((state == "继续采样(含参)"))
//    {
//        ui->comboBox->setCurrentIndex(5);
//        ui->lineEdit_3->setText(strlist.at(3));
//        ui->lineEdit_1->setText(strlist.at(4));
//        ui->lineEdit_2->setText(strlist.at(5));
//    }
    else if((state == "停止采样"))
    {
        ui->comboBox->setCurrentIndex(2);
    }

    WriteUpdate();
}

void collectorOp::updateInfo(ReceiveData *data)
{
    for (int i = 0; i < 32; ++i) {
        QString objName = QString("comboBox_%1").arg(i+1);
        QComboBox* combox = ui->sa_widget->findChild<QComboBox*>(objName);
        if (combox) {
            if(data->collector[i] < combox->count())
            {
                combox->setCurrentIndex(data->collector[i]);
            }
        }
    }

    if(data->collector[32] == 0xff)
    {
        ui->stateBox->setCurrentIndex(0);
    }
    else if(data->collector[32] == 0xe0)
    {
        ui->stateBox->setCurrentIndex(1);
    }
    else if(data->collector[32] == 0x00)
    {
        ui->stateBox->setCurrentIndex(2);
    }
    else if(data->collector[32] == 0x01)
    {
        ui->stateBox->setCurrentIndex(3);
    }
    else if(data->collector[32] == 0x0a)
    {
        ui->stateBox->setCurrentIndex(4);
    }

    if(user_state)
    {
        user_state = false;
        int tmp = 0;
        float tmp_f = 0.0;
        QByteArray byteArray(4, 0);

        tmp = (data->collector[34]<<8)| data->collector[35];
        ui->lineEdit_1->setText(QString::number(tmp));   //34
        tmp = (data->collector[36]<<8)| data->collector[37];
        ui->lineEdit_2->setText(QString::number(tmp));   //36
        tmp = (data->collector[38]<<8)| data->collector[39];
        ui->lineEdit_3->setText(QString::number(tmp));  //38

        if(data->collector[41] == 0x00)
        {
            ui->comboBox->setCurrentIndex(0);
        }
        else if(data->collector[41] == 0x01)
        {
            ui->comboBox->setCurrentIndex(1);
        }
        else if(data->collector[41] == 0xff)
        {
            ui->comboBox->setCurrentIndex(2);
        }
        else if(data->collector[41] == 0x0a)
        {
            ui->comboBox->setCurrentIndex(3);
        }
        else if(data->collector[41] == 0x0b)
        {
            ui->comboBox->setCurrentIndex(4);
        }
//        else if(data->collector[41] == 0x0c)
//        {
//            ui->comboBox->setCurrentIndex(5);
//        }

        memcpy(byteArray.data(), &(data->collector[42]), 4);
        tmp_f = strDecoder::bigEndianToFloat(byteArray);
        ui->lineEdit_5->setText(QString::number(tmp_f));  //42
        memcpy(byteArray.data(), &(data->collector[46]), 4);
        tmp_f = strDecoder::bigEndianToFloat(byteArray);
        ui->lineEdit_6->setText(QString::number(tmp_f));  //46
    }
}

void collectorOp::readyTorun(const QString& str)
{
    emit cmdTorun(str);
}

void collectorOp::on_readbtn_clicked()
{
    emit cmdTorun("收集器 0 参数读取 0 5 5 16 255 1.0 1.0");
    QTimer::singleShot(6000, this, [this]() {
            user_state = true;
        });
}

void collectorOp::on_stopbtn_clicked()
{
    ui->comboBox->setCurrentIndex(2);

    WriteUpdate();
}

void collectorOp::on_writebtn_clicked()
{
    MyMessageBox:: StandardButton result = MyMessageBox::information(this,"设置收集器参数?","注意!!!",MyMessageBox::Yes | MyMessageBox::No);
    switch (result)
    {
        case MyMessageBox::Yes:
             break;
        case MyMessageBox::No:
        default:
             return ;
    }

    WriteUpdate();
}

void collectorOp::WriteUpdate()
{
    int value = 0;
    if(ui->comboBox->currentIndex() == 2)
    {
        value = 0xff;
        run_flag = false;
    }
    else if(ui->comboBox->currentIndex() == 3)
    {
        value = 0x0a;
    }
    else if(ui->comboBox->currentIndex() == 4)
    {
        value = 0x0b;
        type = false;
        if(run_flag)
        {
            condition++;
        }
    }
//    else if(ui->comboBox->currentIndex() == 5)
//    {
//        value = 0x0c;
//        type = true;

//        samle[con_cnt] = ui->lineEdit_3->text().toInt();
//        if(con_cnt++ == COLL_CNT)
//        {
//            con_cnt = 0;
//        }
////        if(run_flag)
////        {
////            condition++;
////        }
//    }
    else
    {
        value = ui->comboBox->currentIndex();
        mode = ui->comboBox->currentIndex();
        labelInit();
        run_flag = true;
        condition = 1;
        con_cnt = 0;
        fin_cnt = 0;
    }

    emit cmdTorun("收集器 0 参数设置 1 "+ui->lineEdit_1->text()+" "+ui->lineEdit_2->text()+" "+ui->lineEdit_3->text()+" "+QString::number(value)+" "+ui->lineEdit_5->text()+" "+ui->lineEdit_6->text());
}

void collectorOp::LabelUpdate(int index)
{
    if(!run_flag) return;

    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (!comboBox) return;

    QString comboName = comboBox->objectName();
    int comboNum = comboName.remove("comboBox_").toInt();

    int labelNum = comboNum + 39;
    QString labelName = QString("label_%1").arg(labelNum);
    QLabel* label = this->findChild<QLabel*>(labelName);
    if (!label) return;

    QString displayText;
    if(index == 2)
    {
        if(!mode)
        {
            displayText = "条件"+QString::number(condition);
            fin_cnt ++;
        }
    }
    else if(index == 4)
    {
        if(mode)
        {
            displayText = "条件"+QString::number(condition);
            fin_cnt ++;
        }
    }
    else
    {
        displayText = "--";
    }

//    if(type && fin_cnt == samle[condition-1])
//    {
//        if(condition++ > COLL_CNT)
//        {
//            condition = 1;
//        }
//        fin_cnt = 0;
//    }

    label->setText(displayText);
}

void collectorOp::SampleUpdate(int index)
{
    if(index == 0)
    {
        if(ui->lineEdit_3->text().toInt() > 32)
        {
            ui->lineEdit_3->setText("32");
        }
    }
    else if(index == 1)
    {
        if(ui->lineEdit_3->text().toInt() > 16)
        {
            ui->lineEdit_3->setText("16");
        }
    }
}

bool collectorOp::check_run(const QString& str)
{
    if(auto_Run)
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
                background-color: #00ffff;
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
                background-color: #00ffff;
            }
        )");

        msgBox.setText(QString("是否继续执行命令：\n%1").arg(str));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if(msgBox.exec() != QMessageBox::Yes)
        {
            return false;
        }
    }
    cmdTorun(str);

    return true;
}
