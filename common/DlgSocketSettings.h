#ifndef __DlgSocketSettings_H__
#define __DlgSocketSettings_H__

#include <QDialog>

namespace Ui {
    class DlgSocketSettings;
}

class QTcpSocket;
class DlgSocketSettings : public QDialog
{
public:
    explicit DlgSocketSettings(QWidget *parent = 0);
    ~DlgSocketSettings();

    QString GetHost()const;
    uint16_t GetPort()const;
    void Inital(const QString &host, int port);
private:
    void initUi();
private:
    Ui::DlgSocketSettings   *m_ui;
};

#endif //__DlgFeedMaterial_H__
