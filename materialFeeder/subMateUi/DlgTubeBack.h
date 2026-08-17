#ifndef __DlgTubeBack_H__
#define __DlgTubeBack_H__

#include <QDialog>

namespace Ui {
    class DlgTubeBack;
}

class TubeStruct;
class DlgTubeBack : public QDialog
{
public:
    explicit DlgTubeBack(QWidget *parent = 0);
    ~DlgTubeBack();

    void Init(const TubeStruct *bt);
private:
    void initUi();
private:
    Ui::DlgTubeBack    *m_ui;
};

#endif //__DlgTubeBack_H__
