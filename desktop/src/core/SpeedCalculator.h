#ifndef SPEEDCALCULATOR_H
#define SPEEDCALCULATOR_H

#include <QObject>
#include <QElapsedTimer>
#include <QList>
#include <QPair>

class SpeedCalculator : public QObject
{
    Q_OBJECT

public:
    explicit SpeedCalculator(QObject *parent = nullptr);
    ~SpeedCalculator();

    void start();
    void stop();
    void reset();
    void updateProgress(qint64 bytesDownloaded);

    double getCurrentSpeed() const; // bytes per second
    double getAverageSpeed() const; // bytes per second
    qint64 getTotalDownloaded() const;
    QString getFormattedSpeed(double speed) const;

signals:
    void speedUpdated(double currentSpeed, double averageSpeed);

private:
    QElapsedTimer m_timer;
    QList<QPair<qint64, qint64>> m_progressHistory; // time (ms), bytes
    qint64 m_totalDownloaded;
    bool m_isRunning;

    static const int HISTORY_SIZE = 10; // Keep last 10 measurements for current speed
};

#endif // SPEEDCALCULATOR_H