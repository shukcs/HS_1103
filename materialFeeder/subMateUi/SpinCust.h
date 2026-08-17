#ifndef __SpinCust_H__
#define __SpinCust_H__

#include <QWidget>

namespace Ui {
    class SpinCust;
}

class SpinCust : public QWidget
{
    Q_OBJECT
public:
    explicit SpinCust(QWidget *parent = 0);
    ~SpinCust();

    void SetWeight(double w);
    void SetMaxWeight(double w);
    double GetWeight()const;
    void SetBtnVisible(bool b, int idx = 0);
private:
signals:
    void btnClick(int idx);//0:+, 1:del
private:
    Ui::SpinCust*m_ui;
};

#endif //__DlgMaterialModify_H__
