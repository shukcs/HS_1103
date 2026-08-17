#include "MotorGroupBox.h"
#include "strDecoder/strdecoder.h"

#include "Ui_MotorGroupBox.h"
#pragma execution_character_set("utf-8")

MotorGroupBox::MotorGroupBox(MotorType tp, QWidget *parent) : QGroupBox(parent)
, m_ui(new Ui::MotorGroupBox), m_type(tp)
{
    m_ui->setupUi(this);
    connect(m_ui->pushButton, &QPushButton::clicked, this, &MotorGroupBox::onAdd);
    initType(tp);
    connect(m_ui->btn_test, &QPushButton::clicked, this, [=] {
        if (auto dec = strDecoder::Instance())
            dec->strTocmd(title() + _getChStr() + _getBracerStr() + _getDirString());
    });
}

void MotorGroupBox::initType(MotorType tp)
{
    switch (tp)
    {
    case MotorGroupBox::Motor_57:
        initFurnaceUi();
        break;
    case MotorGroupBox::Motor_86:
        initReactionTubeUi();
        break;
    case MotorGroupBox::Motor_robot:
        initRobotUi();
        break;
    default:
        break;
    }
}

void MotorGroupBox::initReactionTubeUi()
{
    setTitle(tr("反应管"));
    m_ui->cmb_get->clear();
    m_ui->cmb_get->addItems({ tr("反应管上升"), tr("反应管下降") });
    m_ui->pushButton->setVisible(false);
}

void MotorGroupBox::initFurnaceUi()
{
    setTitle(tr("炉膛"));
    m_ui->cmb_get->clear();
    m_ui->cmb_get->addItems({ tr("打开炉膛"), tr("闭合炉膛") });
    m_ui->cmb_get->setCurrentIndex(0);
    m_ui->pushButton->setVisible(false);
}

void MotorGroupBox::initRobotUi()
{
    setTitle(tr("机械臂滑轨"));
    m_ui->label->setVisible(false);
    m_ui->cmb_ch->setVisible(false);
    m_ui->cmb_get->clear();
    m_ui->cmb_get->addItems({ tr("取放反应管"), tr("取放料仓"), tr("进出天平"), tr("天平门开关"), tr("炉膛1"), tr("原点") });
    m_ui->lb_pos->setText(tr("工作点"));
    m_ui->lb_bracer->setVisible(false);
    m_ui->cmb_bracer->setVisible(false);
    m_ui->pushButton->setVisible(false);
}

QString MotorGroupBox::_getChStr() const
{
    if (Motor_robot != m_type)
    {
        switch (m_ui->cmb_ch->currentIndex())
        {
        case 0:
            return  tr(" 通道 1");
        case 1:
            return  tr(" 通道 2");
        case 2:
            return  tr(" 通道 1 2");
        default:
            break;
        }
    }
    return QString();
}

QString MotorGroupBox::_getBracerStr() const
{
    if (m_type != Motor_robot)
        return tr(" 刹车 ") + QString::number(m_ui->cmb_bracer->currentIndex() == 0 ? 0 : 1);

    return QString();
}

QString MotorGroupBox::_getDirString() const
{
    if (m_type == Motor_robot)
    {
        auto pos = (m_ui->cmb_get->currentIndex() + 1) % m_ui->cmb_get->count();
        return tr(" 工作点 ") + QString::number(pos);
    }

    return tr(" 方向 ") + QString::number(m_ui->cmb_get->currentIndex() == 0 ? 1 : 0);
}

void MotorGroupBox::onAdd()
{
    emit sig_Add(title() + _getChStr() + _getBracerStr() + _getDirString());
}