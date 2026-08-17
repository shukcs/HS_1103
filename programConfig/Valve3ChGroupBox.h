#ifndef __Valve3ChGroupBox_H__
#define __Valve3ChGroupBox_H__

#include <QGroupBox>
namespace Ui {
    class Valve3ChGroupBox;
}
class Valve3ChGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit Valve3ChGroupBox(QWidget *parent=nullptr);
    ~Valve3ChGroupBox();
protected:
    QString _getChStr()const;
    QString _getOutString()const;
signals:
    void sig_Add(const QString &str);
private:
    Ui::Valve3ChGroupBox*  m_ui;
};

#endif // __Valve3ChGroupBox_H__
