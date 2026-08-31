#include "DeviceLog.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>

LogItem::LogItem(const QString &str):dateTime(QDateTime::currentSecsSinceEpoch())
, dsc(str)
{
}

LogItem::LogItem(int64_t tm, const QString &str) : dateTime(tm)
, dsc(str)
{
}

QString LogItem::content()const
{
    return QDateTime::fromSecsSinceEpoch(dateTime).toString("yyyy/M/d hh:mm:ss") + " " + dsc;
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
    emit itemAdded({ m_logs.first() });
}

const QList<LogItem> & DeviceLog::AllLogs() const
{
    return m_logs;
}

void DeviceLog::Export(const QString &file)
{
    QFile f(file);
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream strm(&f);
        for (auto &itr : m_logs)
        {
            strm << itr.dateTime;
            strm << "#$*";
            strm << itr.dsc;
            strm << "\r\n";
        }
        f.close();
    }
}

void DeviceLog::Import(const QString &file)
{
    QFile f(file);
    if (f.open(QIODevice::ReadOnly))
    {
        m_logs.clear();
        emit itemCleared();
        QTextStream strm(&f);
        auto str = strm.readLine(1024);
        while (!str.isEmpty())
        {
            auto tmp = str.indexOf("#$*");
            if (tmp > 0)
                m_logs << LogItem(str.left(tmp).toLongLong(), str.mid(tmp + 3));
            
            str = strm.readLine(1024);
        }
        emit itemAdded(m_logs);
    }
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
