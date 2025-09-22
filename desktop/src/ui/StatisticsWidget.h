#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QTimer>
#include "core/SpeedCalculator.h"
#include "core/DownloadItem.h"

class StatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();

    void setDownloadItem(DownloadItem *item);
    void updateStatistics(qint64 downloaded, qint64 total, double speed);

private slots:
    void updateChart();

private:
    DownloadItem *m_downloadItem;
    SpeedCalculator *m_speedCalculator;

    QLabel *m_speedLabel;
    QLabel *m_etaLabel;
    QLabel *m_downloadedLabel;
    QLabel *m_totalLabel;
    QProgressBar *m_progressBar;

    QChartView *m_chartView;
    QLineSeries *m_speedSeries;
    QDateTimeAxis *m_timeAxis;
    QValueAxis *m_speedAxis;

    QTimer *m_updateTimer;
    QList<QPointF> m_speedHistory;

    void setupUI();
    QString formatSize(qint64 bytes) const;
    QString formatSpeed(double speed) const;
    int calculateETA(qint64 downloaded, qint64 total, double speed) const;
    QString formatTime(int seconds) const;
};

#endif // STATISTICSWIDGET_H