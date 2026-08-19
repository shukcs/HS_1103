#ifndef __MaterialsSelect_H__
#define __MaterialsSelect_H__

#include <QWidget>

namespace Ui {
    class MaterialsSelect;
}

class QComboBox;
class SpinCust;
class FeederParam;
class MaterialsSelect : public QWidget
{
    Q_OBJECT
private:
    typedef struct {
        QComboBox *cmb;
        SpinCust  *spin;
        QWidget   *mask;
    } UnitMate;
public:
    explicit MaterialsSelect(QWidget *parent = 0);
    ~MaterialsSelect();

    void GetFeedMaterials(QMap<QString, float>* mates)const;
    void SetFeedMaterials(const FeederParam *bt);
    int GetChannel()const;
    void SetBottleSelected(bool b);
    int8_t GetTubeNumber()const;
    int GetSelectedBottleNum()const;
private:
    void initUi(QComboBox* cmb, SpinCust* sp);
    void addUnit(SpinCust* sp);
    void delUnit(SpinCust* sp);
    int indexUnit(SpinCust* sp);

    void changeAvalidMate();
    void changeAvalidTube();
    void changeAvalidBottle();
signals:
    void avalidChanged(bool);
private:
    Ui::MaterialsSelect     *m_ui;
    QList<UnitMate>         m_unitMates;
    bool                    m_bVallid=false;
};

#endif //__DlgMaterialModify_H__
