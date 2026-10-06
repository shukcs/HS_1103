#ifndef __HsApplication_H__
#define __HsApplication_H__
#include <QApplication>

class FeederMgr;
class DevContrlMgr;
class portThread;
class RobotMgr;
class deskTop;
class customGraph;
class HsApplication : public QApplication
{
public:
	HsApplication(int &argc, char **argv);
	~HsApplication();

	static QString AppDir(const QString& subDir=QString());
	FeederMgr *feederMgr()const;
	DevContrlMgr *devContrlMgr()const;
	QWidget *mainWidget();
    customGraph *getGustomGraph();
	portThread* getThread()const;
	RobotMgr *GetRobotMgr();
private:
	FeederMgr		*m_feederMgr;
    RobotMgr        *m_robot;
    deskTop			*m_deskTop;
    customGraph     *m_graph = nullptr;
};

#define hsApp static_cast<HsApplication*>(qApp)
#endif // __HsApplication_H__

