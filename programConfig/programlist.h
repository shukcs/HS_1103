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

class QComboBox;
class QPushButton;
class QListWidget;
class ObjList;
class QVBoxLayout;
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
protected:
    void runTitle();
private:
    QComboBox *nameList;
    QPushButton *refresh;
    QPushButton *run;
    QPushButton *pause;
    QListWidget *proList;
    bool state;
    QTimer timer;
    QTimer runTimer;
    int runTime;        //  程序运行时间
    int programCnt;     //  程序条目计数
    int circulationCnt;       // 循环次数
    int totalcirculationCnt;  // 总循环次数
    int delayTime;           // 延时时间(s)
    int CirculateStartLine;  // 开始循环的位置
    int CirculateStopLine;   // 结束循环的位置
    ObjList *obj;
    QVBoxLayout *V2layout;
    QList<ListTextContent> m_titles;
    int m_runIndex = 0;     // 当前运行到第几个大类
    int m_curIndex = -1;     //当前显示的标签
    bool   m_bReady = true;
};

#endif // PROGRAMLIST_H
