#include "tabeledit.h"
#include <QRegularExpressionValidator>
//#pragma execution_character_set("utf-8")

tabelEdit::tabelEdit(QWidget *parent,int row,int col) : QLineEdit(parent)
{
    btn_col = col;
    btn_row = row;
    this->setText("0"); 
    QIntValidator *validator = new QIntValidator(0, 999, this); // 设置浮点数范围为0到999，小数位数为0
    this->setValidator(validator);

    this->setFrame(false);
    this->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // 基础样式
    this->setStyleSheet(
        "QLineEdit {"
        "    border: none;"
        "    background: transparent;"
        "    color: black;"
        "    padding: 0px;"
        "    margin: 0px;"
        "}"
        "QLineEdit:focus {"
        "    border: none;"
        "    background: rgba(240, 240, 240, 0.7);"
        "    color: black;"
        "}"
    );

    connect(this,SIGNAL(editingFinished()),this,SLOT(edit_slot()));
    //connect(&timer,SIGNAL(timeout()),this,SLOT(timerOut()));
}

void tabelEdit::set_pos(int row, int col)
{
    btn_col = col;
    btn_row = row;
}

void tabelEdit::edit_slot()
{
    emit edit_finished(btn_row,btn_col,this->text().toFloat());
    //this->setStyleSheet("QLineEdit{background-color:rgb(255,0,0)}");  //  初始化已设置样式，无需额外处理
    //timer.start(500);
}

void tabelEdit::timerOut()
{
    //timer.stop();
    //this->setStyleSheet("QLineEdit{background-color:rgb(255,255,255)}");  //  初始化已设置样式，无需额外处理
}
