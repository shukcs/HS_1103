#ifndef __DeviceLog_H__
#define __DeviceLog_H__
#include <QObject>

class LogItem {
public:
    LogItem(const QString &str);
    QString content()const;
private:
    int64_t dateTime;
    QString dsc;
};

class DeviceLog : public QObject
{
    Q_OBJECT
public:
    ~DeviceLog();

    void Add(const QString &log);
    const QList<LogItem> &AllLogs()const;
    DeviceLog &operator << (const QString &log);

    static DeviceLog &Instance();
protected:
    DeviceLog(QObject *p = nullptr);
signals:
    void itemAdded(const LogItem &);
private:
    QList<LogItem> m_logs;
};

#endif