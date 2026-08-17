#ifndef QCIRCULATION_H
#define QCIRCULATION_H

#include <QGroupBox>

class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QCirculation : public QGroupBox
{
    Q_OBJECT
public:
    explicit QCirculation(QWidget *parent = nullptr);
    QComboBox *CirculList;
    QLineEdit *TimesInput;
    QLabel *unit;
    QPushButton *addBtn;
signals:
    void circulPanelAdd(QString str);
public slots:
    void addBtn_clicked(void);
};

#endif // QCIRCULATION_H
