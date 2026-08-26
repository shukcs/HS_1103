#include "DeviceLog.h"
#include <QDateTime>

LogItem::LogItem(const QString &str):dateTime(QDateTime::currentSecsSinceEpoch())
, dsc(str)
{
}

QString LogItem::content()const
{
    return QDateTime::fromMSecsSinceEpoch(dateTime).toString("yyyy/M/d hh:mm:ss") + " " + dsc;
}

/*
*DeviceLog
*/
DeviceLog::DeviceLog(QObject *p) : QObject(p)
{
}

DeviceLog::~DeviceLog()
{
}

void DeviceLog::Add(const QString &log)
{
    if (log.isEmpty())
        return;

    m_logs.push_front(LogItem(log));
    emit itemAdded(m_logs.first());
}

const QList<LogItem> & DeviceLog::AllLogs() const
{
    return m_logs;
}

DeviceLog & DeviceLog::operator<<(const QString &log)
{
    Add(log);
    return *this;
}

DeviceLog & DeviceLog::Instance()
{
    static DeviceLog sLogs(nullptr);
    return sLogs;
}
