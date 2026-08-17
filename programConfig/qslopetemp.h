#ifndef QSLOPETEMP_H
#define QSLOPETEMP_H

#include <QGroupBox>
class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QSlopeTemp : public QGroupBox
{
    Q_OBJECT
public:
    explicit QSlopeTemp(QWidget *parent = nullptr);

    QComboBox *slopeTempList;   // 列表
    QLineEdit *temp1Input;      // 起始温度
    QLineEdit *temp2Input;      // 目标温度
    QLineEdit *timeInput;       // 升温时间
    QLabel *name[3];            // 名称标签
    QLabel *unit[3];            //  单位
    QPushButton *addBtn;        //  添加
    QPushButton *helpBtn;       // 使用说明

signals:
    void slopePanelAdd(QString str);

public slots:
    void helpBtn_clicked(void);
    void addBtn_clicked(void);
};

#endif // QSLOPETEMP_H
