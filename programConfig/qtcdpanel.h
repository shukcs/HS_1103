#ifndef QTCDPANEL_H
#define QTCDPANEL_H

#include <QGroupBox>

class QLineEdit;
class QLabel;
class QPushButton;
class QTcdpanel : public QGroupBox
{
    Q_OBJECT
public:
    explicit QTcdpanel(QWidget *parent = nullptr);

    QLineEdit *tempInput;      // TCD温度
    QLineEdit *currentInput;      // TCD桥流
    QLabel *name[2];            // 名称标签
    QLabel *unit[2];            //  单位
    QPushButton *addBtn[2];        //  添加

signals:
    void tcdPanelAdd(QString str);

public slots:
    void addBtn1_clicked(void);
    void addBtn2_clicked(void);
};

#endif // QTCDPANEL_H
