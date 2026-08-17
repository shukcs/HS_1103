#ifndef PROJECTMODE_H
#define PROJECTMODE_H

#include <QWidget>
#include "common/mymessageBox.h"
#include "strDecoder/portthread.h"


namespace Ui {
class projectMode;
}

class projectMode : public QWidget
{
    Q_OBJECT

public:
    explicit projectMode(QWidget *parent = 0);
    ~projectMode();

    void updateInfo(ReceiveData *data);

signals:
    void cmdTorun(const QString &str);

public slots:
    void update_user_set(bool state);

private slots:
    void on_flow1_range_editingFinished();
    void on_flow2_range_editingFinished();
    void on_liquid_range_editingFinished();
    void on_liquid2_range_editingFinished();
    void on_pump_cali_editingFinished();
    void on_pump2_cali_editingFinished();

    void on_pres1_range_editingFinished();
    void on_pres2_range_editingFinished();
    void on_pres3_range_editingFinished();
    void on_pres4_range_editingFinished();

    void on_update_clicked();
    void on_update_plugin_clicked();
    void on_state_clicked();

private:
    Ui::projectMode *ui;
    bool user_state = false;
};

#endif // PROJECTMODE_H
