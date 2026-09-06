#ifndef QSFUNCTION_H
#define QSFUNCTION_H

#include <QGroupBox>

namespace Ui {
	class QSFunction;
}
class QSFunction : public QGroupBox
{
    Q_OBJECT
public:
    explicit QSFunction(QWidget *parent = nullptr);
	~QSFunction();
signals:
    void functionPanelAdd(const QString &);
private:
	void initUi();
	void onRec();
	void onDelay();
	void onCycle();
private:
	Ui::QSFunction *m_ui;
};

#endif // QSFUNCTION_H
