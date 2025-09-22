#include "SpeedCalculator.h"
#include <QDebug>

SpeedCalculator::SpeedCalculator(QObject *parent)
    : QObject(parent)
    , m_totalDownloaded(0)
    , m_isRunning(false)
{
}

SpeedCalculator::~SpeedCalculator()
{
}

void SpeedCalculator::start()
{
    if (!m_isRunning) {
        m_timer.start();
        m_isRunning = true;
        m_progressHistory.clear();
        m_totalDownloaded = 0;
    }
}

void SpeedCalculator::stop()
{
    m_isRunning = false;
}

void SpeedCalculator::reset()
{
    m_progressHistory.clear();
    m_totalDownloaded = 0;
    m_timer.restart();
}

void SpeedCalculator::updateProgress(qint64 bytesDownloaded)
{
    if (!m_isRunning) {
        return;
    }

    qint64 elapsed = m_timer.elapsed();
    m_progressHistory.append(qMakePair(elapsed, bytesDownloaded));
    m_totalDownloaded = bytesDownloaded;

    // Keep only last HISTORY_SIZE entries
    if (m_progressHistory.size() > HISTORY_SIZE) {
        m_progressHistory.removeFirst();
    }

    double currentSpeed = getCurrentSpeed();
    double averageSpeed = getAverageSpeed();
    emit speedUpdated(currentSpeed, averageSpeed);
}

double SpeedCalculator::getCurrentSpeed() const
{
    if (m_progressHistory.size() < 2) {
        return 0.0;
    }

    // Calculate speed based on last two measurements
    auto last = m_progressHistory.last();
    auto secondLast = m_progressHistory.at(m_progressHistory.size() - 2);

    qint64 timeDiff = last.first - secondLast.first;
    qint64 bytesDiff = last.second - secondLast.second;

    if (timeDiff == 0) {
        return 0.0;
    }

    return (bytesDiff * 1000.0) / timeDiff; // bytes per second
}

double SpeedCalculator::getAverageSpeed() const
{
    if (m_progressHistory.isEmpty() || m_timer.elapsed() == 0) {
        return 0.0;
    }

    qint64 totalTime = m_timer.elapsed();
    return (m_totalDownloaded * 1000.0) / totalTime; // bytes per second
}

qint64 SpeedCalculator::getTotalDownloaded() const
{
    return m_totalDownloaded;
}

QString SpeedCalculator::getFormattedSpeed(double speed) const
{
    if (speed < 1024) {
        return QString("%1 B/s").arg(speed, 0, 'f', 0);
    } else if (speed < 1024 * 1024) {
        return QString("%1 KB/s").arg(speed / 1024, 0, 'f', 1);
    } else if (speed < 1024 * 1024 * 1024) {
        return QString("%1 MB/s").arg(speed / (1024 * 1024), 0, 'f', 1);
    } else {
        return QString("%1 GB/s").arg(speed / (1024 * 1024 * 1024), 0, 'f', 1);
    }
}