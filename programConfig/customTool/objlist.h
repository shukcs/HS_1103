#ifndef OBJLIST_H
#define OBJLIST_H

#include <QWidget>

class QScrollArea;
class QVBoxLayout;
class ObjList : public QWidget
{
    Q_OBJECT
public:
    explicit ObjList(QWidget *parent = nullptr);
    ~ObjList();
    void initComponent();

    void add_Btn(QString name,int index);
    void list_clear();

    void setRun(int index);
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
    int m_idxRun;
};

#endif // OBJLIST_H
