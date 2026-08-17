#ifndef __MotorGroupBox_H__
#define __MotorGroupBox_H__

#include <QGroupBox>
namespace Ui {
    class MotorGroupBox;
}
class MotorGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    enum MotorType
    {
        Motor_35,
        Motor_Pipet = Motor_35,
        Motor_57,
        Motor_86,
        Motor_robot, //机械臂滑轨
    };
public:
    explicit MotorGroupBox(MotorType tp=Motor_35, QWidget *parent = nullptr);
protected:
    void initType(MotorType tp);
    void initReactionTubeUi();
    void initFurnaceUi();
    void initRobotUi();
    QString _getChStr()const;
    QString _getBracerStr()const;
    QString _getDirString()const;
signals:
    void sig_Add(const QString &str);
 protected slots:
     void onAdd();
private:
    Ui::MotorGroupBox*  m_ui;
    MotorType           m_type;
};

#endif // __MotorGroupBox_H__
