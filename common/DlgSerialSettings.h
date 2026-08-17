#ifndef __DlgSerialSettings_H__
#define __DlgSerialSettings_H__

#include <QDialog>

namespace Ui {
    class DlgSerialSettings;
}

class QSerialPort;
class DlgSerialSettings : public QDialog
{
public:
    explicit DlgSerialSettings(QWidget *parent = 0);
    ~DlgSerialSettings();

    void Inital(QSerialPort *port, bool bBase=true);
private:
    void initUi();
    void connectPort();
private:
    Ui::DlgSerialSettings   *m_ui;
    QSerialPort             *m_port = nullptr;
    bool                    m_bChanged = false;
};

#endif //__DlgFeedMaterial_H__
