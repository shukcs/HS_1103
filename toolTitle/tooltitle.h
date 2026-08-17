#ifndef TOOLTITLE_H
#define TOOLTITLE_H

#include <QWidget>

namespace Ui {
class toolTitle;
}

class toolTitle : public QWidget
{
    Q_OBJECT

public:
    explicit toolTitle(QWidget *parent = 0);
    ~toolTitle();

signals:
    void logoBtn_clicked();

private slots:
    void logoBtn_slot();
private:
    Ui::toolTitle *ui;
};

#endif // TOOLTITLE_H
