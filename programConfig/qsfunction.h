#ifndef QSFUNCTION_H
#define QSFUNCTION_H

#include <QGroupBox>
class QComboBox;
class QLabel;
class QPushButton;
class QSFunction : public QGroupBox
{
    Q_OBJECT
public:
    explicit QSFunction(QWidget *parent = nullptr);
    QComboBox *actionList;
    QLabel *name[5];            // 名称标签
    QPushButton *addBtn[5];        //  添加
signals:
    void functionPanelAdd(QString str);

public slots:
    void addBtn1_clicked(void);
    void addBtn2_clicked(void);
    void addBtn3_clicked(void);
    void addBtn4_clicked(void);
    void addBtn5_clicked(void);
};

#endif // QSFUNCTION_H
