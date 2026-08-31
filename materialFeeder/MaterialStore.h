#ifndef __MaterialStore_H__
#define __MaterialStore_H__

#include <QWidget>

namespace Ui {
    class MaterialStore;
}

class RobotMgr;
class StoreStruct;
class MaterialStore : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialStore(QWidget *parent = 0);
    ~MaterialStore();

    RobotMgr *GetRobotMgr()const;
private slots:
    void updateStore(const StoreStruct* c);
private:
    void initFeederDecode();
    void initUi();
    void initRobotStat();
    void initFeederStat();
    void initLog();
private:
    Ui::MaterialStore       *m_ui;
    RobotMgr                *m_robot;
};

#endif //__MaterialStore_H__
