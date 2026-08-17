#ifndef HSLIST_H
#define HSLIST_H

#include <QListWidget>

class HsList : public QListWidget
{
    Q_OBJECT
public:
    explicit HsList(QWidget *parent = nullptr);

    void dropEvent(QDropEvent *event);
    void performDrag(void);

signals:

public slots:
    void DeleteWarning(QListWidgetItem* item);
};

#endif // HSLIST_H
