#ifndef __AirWayGroupBox_H__
#define __AirWayGroupBox_H__

#include <QGroupBox>
namespace Ui {
    class AirWayGroupBox;
}
class AirWayGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit AirWayGroupBox(QWidget *parent = nullptr);
    ~AirWayGroupBox();
protected:
    void initUi();

    void onClear();
    void onAirIn();
signals:
    void sig_Add(const QString &str);
private:
    Ui::AirWayGroupBox*  m_ui;
};

#endif // __AirWayGroupBox_H__
