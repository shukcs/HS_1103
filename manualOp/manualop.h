#ifndef MANUALOP_H
#define MANUALOP_H

#include <QWidget>

namespace Ui {
    class manualOp;
}
class ReceiveData;
class diagram;
class manualOp : public QWidget
{
    Q_OBJECT

public:
    explicit manualOp(QWidget *parent = 0);
    ~manualOp();

    void updateInfo(ReceiveData *data);
    diagram* getDiagram();
signals:
    void cmdTorun(const QString &str);
private slots:
    void readyTorun(const QString& str);
    void strToState(const QString& cmd);   // 调整图标到相应状态
    void autoRun(bool state);       // 更新运行模式
private:
    Ui::manualOp *ui;
};

#endif // MANUALOP_H
