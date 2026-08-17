#ifndef OBJLIST_H
#define OBJLIST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QLayoutItem>
#include <QScrollArea>
#include <QScroller>

class objList : public QWidget
{
    Q_OBJECT
public:
    explicit objList(QWidget *parent = nullptr);
    ~objList();
    void initComponent();

    void add_Btn(QString name,int index);
    void list_clear();

    void setBtn_Enable(int index);
    void setAllBtn_Enable();

signals:

    void obj_clicked(int index);

public slots:
//    void addWidget(QWidget *w);

    void btn_clicked(void);

private:
    QScrollArea* m_pChatListScrollArea;
    QVBoxLayout* m_pSCVLayout;

    QWidget* widget;
};

#endif // OBJLIST_H
