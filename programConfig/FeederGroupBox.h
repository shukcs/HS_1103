#ifndef __FeederGroupBox_H__
#define __FeederGroupBox_H__

#include <QGroupBox>
namespace Ui {
    class FeederGroupBox;
}
class TubeStruct;
class FeederGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit FeederGroupBox(QWidget *parent=nullptr);
    ~FeederGroupBox();
protected:
    QString _getFixStr()const; 
    QString _getBackStr()const;
	QString _getFeedChStr()const;
	QString _getFeedMateStr()const;
    void initUi();

    void addTube(const TubeStruct* tube);
signals:
    void sig_Add(const QString &str);
 protected slots:
	void onAdd();
	void onTubeBack();
    void onFix();
private:
    Ui::FeederGroupBox*  m_ui;
};

#endif // __FeederGroupBox_H__
