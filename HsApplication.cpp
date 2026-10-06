#include "HsApplication.h"
#include <QDir>
#include "desktop.h"
#include "customGraph/customgraph.h"
#include "DevContrlMgr/DevContrlMgr.h"
#include "materialFeeder/FeederMgr.h"
#include "materialFeeder/RobotMgr.h"

HsApplication::HsApplication(int &argc, char **argv) : QApplication(argc, argv)
, m_feederMgr(new FeederMgr(this)), m_deskTop(nullptr), m_robot(nullptr)
{
}

HsApplication::~HsApplication()
{
	delete m_feederMgr;
	delete m_robot;
}

QString HsApplication::AppDir(const QString& subDir)
{
	QDir dir(applicationDirPath() + "/" + subDir);
	if (!dir.exists())
		dir.mkdir(dir.absolutePath());

	return dir.absolutePath();
}

FeederMgr * HsApplication::feederMgr()const
{
	return m_feederMgr;
}

DevContrlMgr * HsApplication::devContrlMgr() const
{
	return m_deskTop ? m_deskTop->getDevContrlMgr() : nullptr;
}

QWidget * HsApplication::mainWidget()
{
	if (!m_deskTop)
		m_deskTop = new deskTop;

	return m_deskTop;
}

customGraph * HsApplication::getGustomGraph()
{
    if (!m_graph)
        m_graph = new customGraph();

    return m_graph;
}

portThread* HsApplication::getThread() const
{
	if (auto dev = devContrlMgr())
		return dev->getThread();

	return nullptr;
}

RobotMgr * HsApplication::GetRobotMgr()
{
	if (!m_robot)
		m_robot = new RobotMgr(this);

	return m_robot;
}
