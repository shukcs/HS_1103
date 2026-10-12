#ifndef __FinishWorkGroupBox_H__
#define __FinishWorkGroupBox_H__

#include <QGroupBox>
namespace Ui {
    class FinishWorkGroupBox;
}
class FinishWorkGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit FinishWorkGroupBox(QWidget *parent=nullptr);
    ~FinishWorkGroupBox();

private:
    Ui::FinishWorkGroupBox*  m_ui;
};

#endif // __Valve3ChGroupBox_H__
