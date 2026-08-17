#ifndef TABELEDIT_H
#define TABELEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QDebug>
#include <QTimer>

class tabelEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit tabelEdit(QWidget *parent = nullptr,int row = 0,int col = 0);
    void set_pos(int row,int col);
signals:

    void edit_finished(int row,int col,float val);

public slots:
    void edit_slot(void);
    void timerOut(void);
private:
    int btn_row;
    int btn_col;
    //QTimer timer;
};

#endif // TABELEDIT_H
