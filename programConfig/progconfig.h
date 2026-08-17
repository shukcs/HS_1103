#ifndef PROGCONFIG_H
#define PROGCONFIG_H

#include <QWidget>

#include "customTool/fileexport.h"
#include "customTool/filesave.h"
#include "customTool/fileopen.h"

namespace Ui {
class progConfig;
}

class progConfig : public QWidget
{
    Q_OBJECT

public:
    explicit progConfig(QWidget *parent = 0);
    ~progConfig();

private slots:
    void listAdd(const QString &str);

    void on_btn_open_clicked(void);
    void on_btn_save_clicked(void);
    void on_btn_clear_clicked(void);
    void on_btn_append_clicked(void);
    void on_move_up_clicked();
    void on_move_down_clicked();
    void on_obj_del_clicked();

private:
    Ui::progConfig *ui;
};

#endif // PROGCONFIG_H
