#ifndef PROGRAMLIST_H
#define PROGRAMLIST_H

#include <QWidget>
#include <QTimer>

class ListTextContent
{
public:
   QString name;
   QList<QString> list;
};

class objList;
class QVBoxLayout;
class QComboBox;
class QPushButton;
class QListWidget;
class ProgramList : public QWidget
{
    Q_OBJECT
public:
    ProgramList(QWidget *parent = 0);
    ~ProgramList();

    void state_change(void);
    bool getState(void);

signals:
    void readyTorun(const QString& str);
    void readyTorun_toColl(const QString& str);
    void sendRunTime(int time);
    void autoRun(bool run);

public slots:
    void refreshBtn_clicked(void);
    void runBtn_clicked(void);
    void pauseBtn_clicked(void);
    void timer_slot(void);
    void nameFile_changed(const QString &str);
    void runTimer_slot(void);
    void clear_sub_list();
    void obj_clicked(int index);
    void OnStoveTubeChanged(uint16_t type, uint16_t idx);
private:
    bool state;
    QTimer timer;
    QTimer runTimer;
    int runTime;   //  程序运行时间
    int programCnt;   //  程序条目计数
    int totalCnt;   //  总数量
    int circulationCnt;  // 循环次数
    int totalcirculationCnt;  // 总循环次数
    int delayTime;  // 延时时间(s)
    int CirculateStartLine;  // 开始循环的位置
    int CirculateStopLine;   // 结束循环的位置
    objList *_obj;
    QVBoxLayout *V2layout;
    QList<ListTextContent> ListMain;
    int total_obj = 0;
    int obj_index = 0; // 当前运行到第几个大类
    int run_index; // 每个大类的计数器
    int sub_num; // 子类数量

    QComboBox* nameList;
    QPushButton* refresh;
    QPushButton* _btnrun;
    QPushButton* pause;
    //    QPushButton *circulate;

    QListWidget* _proList;
    bool   m_bReady = true;
};

#endif // PROGRAMLIST_H
