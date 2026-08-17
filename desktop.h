#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>

namespace Ui {
class deskTop;
}
class MaterialStore;
class ReceiveData;
class manualOp;
class deviceOp;
class customGraph;
class progConfig;
class strDecoder;
class projectMode;
class collectorOp;
class deskTop : public QWidget
{
    Q_OBJECT

public:
    explicit deskTop(QWidget *parent = 0);
    ~deskTop();
public slots:
    void Scene1Show();
    void Scene2Show();
    void Scene3Show();
    void Scene4Show();
    void Scene5Show();
    void Scene6Show();
    void StoreShow();
    void updateInfo(ReceiveData *data);
    void updateRunTime(int time);
private slots:
    void on_configBtn_clicked();

private:
    Ui::deskTop     *ui;
    MaterialStore   *m_mateSt;
    manualOp *manualOperation;
    deviceOp *deviceOperation;
    customGraph *graph;
  //  ProgramConfig *programconfig;
    progConfig *progconfig;
    strDecoder *decoder;
    projectMode *projectmode;
    collectorOp *collectorOperation;
};

#endif // DESKTOP_H
