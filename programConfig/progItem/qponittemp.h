#ifndef QPONITTEMP_H
#define QPONITTEMP_H

#include <QGroupBox>

class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QPonitTemp : public QGroupBox
{
    Q_OBJECT
public:
    explicit QPonitTemp(QWidget *parent = nullptr);
    QComboBox *pointTempList;
    QLineEdit *tempInput;
    QLabel *unit;
    QPushButton *addBtn;


signals:
    void pointTempPanelAdd(QString str);

public slots:
    void addBtn_clicked(void);
};

#endif // QPONITTEMP_H
