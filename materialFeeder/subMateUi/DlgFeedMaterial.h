#ifndef __DlgFeedMaterial_H__
#define __DlgFeedMaterial_H__

#include <QDialog>

namespace Ui {
    class DlgFeedMaterial;
}

class QComboBox;
class SpinCust;
class FeederParam;
class DlgFeedMaterial : public QDialog
{
    Q_OBJECT
private:
    typedef struct {
        QComboBox   *cmb;
        SpinCust    *spin;
    } UnitMate;
public:
    explicit DlgFeedMaterial(QWidget *parent = 0);
    ~DlgFeedMaterial();

    void Init(const FeederParam *bt);
    bool FeedBottle(QMap<QString, float>* mates);
    int GetTubeNumb();
    void SetBottleAvlible(bool b);
    int GetChannel()const;
private:
    void initUi();
signals:
    void bottleAvlibleChanged(bool);
private:
    Ui::DlgFeedMaterial    *m_ui;
};

#endif //__DlgFeedMaterial_H__
