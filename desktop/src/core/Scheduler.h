#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QVariantMap>

class Database;

class Scheduler : public QObject
{
    Q_OBJECT

public:
    explicit Scheduler(Database *database, QObject *parent = nullptr);
    ~Scheduler();

    // Schedule management
    bool addSchedule(const QString &name, const QDateTime &startTime, const QString &url, const QVariantMap &options = QVariantMap());
    bool removeSchedule(int scheduleId);
    bool updateSchedule(int scheduleId, const QDateTime &newTime);
    QList<QVariantMap> getSchedules() const;

    // Control
    void start();
    void stop();
    bool isRunning() const;

signals:
    void scheduleTriggered(int scheduleId, const QString &url, const QVariantMap &options);
    void scheduleError(int scheduleId, const QString &error);

private slots:
    void checkSchedules();

private:
    Database *m_database;
    QTimer *m_checkTimer;
    QList<QVariantMap> m_schedules;

    void loadSchedules();
    void saveSchedules();
};

#endif // SCHEDULER_H