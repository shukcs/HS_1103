#ifndef COLLECTOROP_H
#define COLLECTOROP_H

#include <QWidget>
#include <QFile>
#include <QDir>
#include <QValidator>
#include <QTextStream>
#include <QMessageBox>
#include <QByteArray>
#include <QDataStream>
#include "common/mymessageBox.h"
#include "strDecoder/portthread.h"
#include "strDecoder/strdecoder.h"


#define COLL_CNT    16


namespace Ui {
class collectorOp;
}

class collectorOp : public QWidget
{
    Q_OBJECT

public:
    explicit collectorOp(QWidget *parent = 0);
    ~collectorOp();
    void updateInfo(ReceiveData *data);

signals:
    void cmdTorun(const QString &);
    void selfcmdTorun(const QString &);

public slots:
    void autoRun(bool state);       // 运行状态更新
    void readyTorun(const QString& str);
    void updateCmd(const QString& str);
protected:
    bool eventFilter(QObject* obj, QEvent* event)override;
private slots:
    void on_readbtn_clicked();
    void on_writebtn_clicked();
    void on_stopbtn_clicked();
    void LabelUpdate(int index);
    void SampleUpdate(int index);
    bool check_run(const QString& str);

private:
    Ui::collectorOp *ui;
    QLabel* mask;
    QLabel* mask2;
    bool user_state = true;
    bool auto_Run = false;
    bool run_flag = false;
    int condition = 0;
    bool mode = false;  //采样模式
    bool type = false;  //继续模式
    uint8_t samle[COLL_CNT] = {0};
    uint8_t fin_cnt = 0;
    uint8_t con_cnt = 0;
    void labelInit(void);
    void WriteUpdate();

};

#endif // COLLECTOROP_H
