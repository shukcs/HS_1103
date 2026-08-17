#ifndef FLOWSET_H
#define FLOWSET_H

#include <QWidget>

namespace Ui {
class flowSet;
}

class flowSet : public QWidget
{
    Q_OBJECT

public:
    explicit flowSet(QWidget *parent = 0);
    ~flowSet();
    void set_title(QString name);
    void set_flow_name(QString name,uint8_t id);
    void set_unit(QString name);
    void set_flow_val(const QVariant &val);
signals:
    void flow_to_set(QString str);

private slots:
    void on_ok_clicked();

    void on_back_clicked();

private:
    Ui::flowSet *ui;
    uint8_t dev_id = 0;
};

#endif // FLOWSET_H
