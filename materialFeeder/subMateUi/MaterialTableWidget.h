#ifndef __MaterialTableWidget_H__
#define __MaterialTableWidget_H__

#include <QTableWidget>

class MaterialStruct;
class MaterialTableWidget : public QTableWidget
{
public:
    MaterialTableWidget(QWidget* p);

    void AddMaterial(const MaterialStruct &m);
    void ChangeMaterial(const QString& id, const MaterialStruct& m);
    bool CurrentRowMaterial(MaterialStruct*);
protected:
    bool eventFilter(QObject* obj, QEvent* e)override;
protected:
    int findIdRow(const QString& id);
};

#endif // __MaterialTableWidget_H__