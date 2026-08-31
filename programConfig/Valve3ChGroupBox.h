#ifndef __Valve3ChGroupBox_H__
#define __Valve3ChGroupBox_H__

#include <QGroupBox>
namespace Ui {
    class FeedLiquidGroupBox;
}
class FeedLiquidGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit FeedLiquidGroupBox(QWidget *parent=nullptr);
    ~FeedLiquidGroupBox();
protected:
    QString _getOutString()const;
    void onValueChanged();
signals:
    void sig_Add(const QString &str);
private:
    Ui::FeedLiquidGroupBox*  m_ui;
};

#endif // __Valve3ChGroupBox_H__
