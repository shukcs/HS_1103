#ifndef __HeatGroupBox_H__
#define __HeatGroupBox_H__

#include <QGroupBox>
namespace Ui {
    class HeatGroupBox;
}
class HeatGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    enum Dev_type {
        Dev_heat,
        Dev_montain,
    };
public:
    explicit HeatGroupBox(Dev_type tp=Dev_heat, QWidget *parent=nullptr);
    ~HeatGroupBox();
protected:
    QString _getChStr()const;
    QString _getTmpString()const;
signals:
    void sig_Add(const QString &str);
 protected slots:
     void onAdd();
private:
    Ui::HeatGroupBox*  m_ui;
};

#endif // __HeatGroupBox_H__
