#ifndef __DlgTubeFix_H__
#define __DlgTubeFix_H__

#include <QDialog>

namespace Ui {
    class DlgTubeFix;
}

class TubeStruct;
class DlgTubeFix : public QDialog
{
public:
    explicit DlgTubeFix(QWidget *parent = 0);
    ~DlgTubeFix();

    void Init(const TubeStruct *bt);
private:
    void initUi();
private:
    Ui::DlgTubeFix    *m_ui;
};

#endif //__DlgTubeFix_H__
