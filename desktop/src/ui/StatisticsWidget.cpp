#include "StatisticsWidget.h"
#include <QChart>
#include <QDateTime>
#include <QDebug>

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent)
    , m_downloadItem(nullptr)
    , m_speedCalculator(new SpeedCalculator(this))
{
    setupUI();
    connect(m_updateTimer, &QTimer::timeout, this, &StatisticsWidget::updateChart);
    m_updateTimer->start(1000); // Update every second
}

StatisticsWidget::~StatisticsWidget()
{
}

void StatisticsWidget::setDownloadItem(DownloadItem *item)
{
    m_downloadItem = item;
    if (item) {
        connect(m_speedCalculator, &SpeedCalculator::speedUpdated, this, [this](double current, double avg) {
            m_speedLabel->setText(QString("Speed: %1").arg(formatSpeed(current)));
        });
    }
}

void StatisticsWidget::updateStatistics(qint64 downloaded, qint64 total, double speed)
{
    int eta = 0;
    if (m_downloadItem) {
        m_downloadItem->setDownloadedSize(downloaded);
        m_downloadItem->setTotalSize(total);
        m_downloadItem->setSpeed(static_cast<int>(speed / 1024)); // KB/s
        eta = calculateETA(downloaded, total, speed);
        m_downloadItem->setEta(eta);
    }

    m_downloadedLabel->setText(QString("Downloaded: %1").arg(formatSize(downloaded)));
    m_totalLabel->setText(QString("Total: %1").arg(formatSize(total)));
    double progress = total > 0 ? (downloaded * 100.0 / total) : 0;
    m_progressBar->setValue(static_cast<int>(progress));
    m_etaLabel->setText(QString("ETA: %1").arg(formatTime(eta)));
    m_speedCalculator->updateProgress(downloaded);
}

void StatisticsWidget::updateChart()
{
    if (!m_downloadItem) return;
    double currentSpeed = m_speedCalculator->getCurrentSpeed();
    QDateTime now = QDateTime::currentDateTime();
    m_speedSeries->append(now.toMSecsSinceEpoch(), currentSpeed / 1024); // KB/s
    if (m_speedSeries->count() > 60) { // Keep last 60 seconds
        m_speedSeries->remove(0);
    }
    m_chartView->chart()->update();
}

void StatisticsWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    // Labels
    QHBoxLayout *infoLayout = new QHBoxLayout();
    m_speedLabel = new QLabel("Speed: 0 KB/s");
    m_etaLabel = new QLabel("ETA: --");
    m_downloadedLabel = new QLabel("Downloaded: 0 B");
    m_totalLabel = new QLabel("Total: 0 B");
    infoLayout->addWidget(m_speedLabel);
    infoLayout->addWidget(m_etaLabel);
    infoLayout->addWidget(m_downloadedLabel);
    infoLayout->addWidget(m_totalLabel);
    layout->addLayout(infoLayout);
    // Progress bar
    m_progressBar = new QProgressBar();
    layout->addWidget(m_progressBar);
    // Chart
    m_chartView = new QChartView();
    QChart *chart = new QChart();
    m_speedSeries = new QLineSeries();
    chart->addSeries(m_speedSeries);
    m_timeAxis = new QDateTimeAxis();
    m_timeAxis->setFormat("hh:mm:ss");
    chart->addAxis(m_timeAxis, Qt::AlignBottom);
    m_speedSeries->attachAxis(m_timeAxis);
    m_speedAxis = new QValueAxis();
    m_speedAxis->setTitleText("Speed (KB/s)");
    chart->addAxis(m_speedAxis, Qt::AlignLeft);
    m_speedSeries->attachAxis(m_speedAxis);
    m_chartView->setChart(chart);
    layout->addWidget(m_chartView);
    m_updateTimer = new QTimer(this);
}

QString StatisticsWidget::formatSize(qint64 bytes) const
{
    if (bytes < 1024) return QString("%1 B").arg(bytes);
    else if (bytes < 1024 * 1024) return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    else if (bytes < 1024 * 1024 * 1024) return QString("%1 MB").arg(bytes / (1024.0 * 1024), 0, 'f', 1);
    else return QString("%1 GB").arg(bytes / (1024.0 * 1024 * 1024), 0, 'f', 1);
}

QString StatisticsWidget::formatSpeed(double speed) const
{
    return QString("%1 KB/s").arg(speed / 1024, 0, 'f', 1);
}

int StatisticsWidget::calculateETA(qint64 downloaded, qint64 total, double speed) const
{
    if (speed <= 0 || downloaded >= total) return 0;
    qint64 remaining = total - downloaded;
    return static_cast<int>(remaining / speed);
}

QString StatisticsWidget::formatTime(int seconds) const
{
    if (seconds <= 0) return "--";
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}