#ifndef STOVETABLE_H
#define STOVETABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QList>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include "tabeledit.h"

#define PRO_CNT 10

class  TableTemp
{
public:
    int temp[PRO_CNT];
    int time[PRO_CNT];
    TableTemp()
    {
      memset(temp,0,sizeof(temp));
      memset(time,0,sizeof(time));
    }
};


class StoveTable : public QWidget
{
    Q_OBJECT
public:
    explicit StoveTable(QWidget *parent = nullptr);
    QTableWidget *tableWidget;
    tabelEdit *edit[2][PRO_CNT];   // 2列PRO_CNT行  温度/时间
    //void setTableHeader(QStringList header);

    void setTableFormat(int col,int row); // 设置表格行列

    void getTableTemp(QList<int> *data);
    void getTableTemp2(QList<int> *data);

    void setTabelTemp(int *temp,int *time);
    void saveTableTemp(void);

    void Table_init(void);
    int get_col(void);
    int get_row(void);

signals:
    void stove_refresh(int row,int col,float val);
public slots:
    void edit_finished(int row,int col,float val);
private:
    int p_col; // 列
    int p_row; // 行
    TableTemp table;
    QFile file;
};

#endif // STOVETABLE_H
