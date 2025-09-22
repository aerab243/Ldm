#include "Scheduler.h"
#include "Database.h"
#include <QDebug>

Scheduler::Scheduler(Database *database, QObject *parent)
    : QObject(parent)
    , m_database(database)
    , m_checkTimer(new QTimer(this))
{
    connect(m_checkTimer, &QTimer::timeout, this, &Scheduler::checkSchedules);
    loadSchedules();
}

Scheduler::~Scheduler()
{
    stop();
}

bool Scheduler::addSchedule(const QString &name, const QDateTime &startTime, const QString &url, const QVariantMap &options)
{
    QVariantMap schedule;
    schedule["id"] = m_schedules.size() + 1; // Simple ID generation
    schedule["name"] = name;
    schedule["startTime"] = startTime;
    schedule["url"] = url;
    schedule["options"] = options;
    schedule["active"] = true;

    m_schedules.append(schedule);
    saveSchedules();

    return true;
}

bool Scheduler::removeSchedule(int scheduleId)
{
    for (int i = 0; i < m_schedules.size(); ++i) {
        if (m_schedules[i]["id"].toInt() == scheduleId) {
            m_schedules.removeAt(i);
            saveSchedules();
            return true;
        }
    }
    return false;
}

bool Scheduler::updateSchedule(int scheduleId, const QDateTime &newTime)
{
    for (QVariantMap &schedule : m_schedules) {
        if (schedule["id"].toInt() == scheduleId) {
            schedule["startTime"] = newTime;
            saveSchedules();
            return true;
        }
    }
    return false;
}

QList<QVariantMap> Scheduler::getSchedules() const
{
    return m_schedules;
}

void Scheduler::start()
{
    if (!m_checkTimer->isActive()) {
        m_checkTimer->start(60000); // Check every minute
    }
}

void Scheduler::stop()
{
    if (m_checkTimer->isActive()) {
        m_checkTimer->stop();
    }
}

bool Scheduler::isRunning() const
{
    return m_checkTimer->isActive();
}

void Scheduler::checkSchedules()
{
    QDateTime now = QDateTime::currentDateTime();

    for (int i = m_schedules.size() - 1; i >= 0; --i) {
        QVariantMap schedule = m_schedules[i];
        if (!schedule["active"].toBool()) continue;

        QDateTime startTime = schedule["startTime"].toDateTime();
        if (startTime <= now) {
            emit scheduleTriggered(schedule["id"].toInt(), schedule["url"].toString(), schedule["options"].toMap());
            // Remove one-time schedule
            m_schedules.removeAt(i);
            saveSchedules();
        }
    }
}

void Scheduler::loadSchedules()
{
    // TODO: Load from database
    // m_schedules = m_database->getSchedules();
    m_schedules.clear();
}

void Scheduler::saveSchedules()
{
    // TODO: Save to database
    // for (const QVariantMap &schedule : m_schedules) {
    //     m_database->insertSchedule(schedule);
    // }
}