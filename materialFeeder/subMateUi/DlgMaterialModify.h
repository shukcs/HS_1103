#ifndef __DlgMaterialModify_H__
#define __DlgMaterialModify_H__

#include <QDialog>

class MaterialStruct;
namespace Ui {
    class DlgMaterialModify;
}

class DlgMaterialModify : public QDialog
{
    Q_OBJECT
public:
    explicit DlgMaterialModify(QWidget *parent = 0);
    ~DlgMaterialModify();

    void Modify(const MaterialStruct* m=nullptr, const QString &id=QString());
    QString GetId()const;
    QString GetName()const;
    double GetWeight()const;
    void GetMaterial(MaterialStruct* m)const;
private:
    void initUi();
private:
    Ui::DlgMaterialModify *m_ui;
};

#endif //__DlgMaterialModify_H__
