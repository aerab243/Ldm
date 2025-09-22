#include "ProgressWidget.h"
#include <QDebug>

ProgressWidget::ProgressWidget(DownloadItem *item, QWidget *parent)
    : QWidget(parent)
    , m_downloadItem(item)
{
    setupUI();
    updateProgress();
    connect(m_downloadItem, &DownloadItem::progressChanged, this, &ProgressWidget::updateProgress);
    connect(m_downloadItem, &DownloadItem::statusChanged, this, &ProgressWidget::updateProgress);
}

ProgressWidget::~ProgressWidget()
{
}

void ProgressWidget::updateProgress()
{
    if (!m_downloadItem) return;

    m_filenameLabel->setText(m_downloadItem->getFilename());
    m_statusLabel->setText(QString("Status: %1").arg(m_downloadItem->getStatus()));
    m_speedLabel->setText(QString("Speed: %1 KB/s").arg(m_downloadItem->getSpeed()));
    m_etaLabel->setText(QString("ETA: %1").arg(formatTime(m_downloadItem->getEta())));
    double progress = m_downloadItem->getProgress();
    m_progressBar->setValue(static_cast<int>(progress * 100));
}

void ProgressWidget::onPauseClicked()
{
    if (m_downloadItem) {
        m_downloadItem->setStatus("paused");
    }
}

void ProgressWidget::onResumeClicked()
{
    if (m_downloadItem) {
        m_downloadItem->setStatus("downloading");
    }
}

void ProgressWidget::onCancelClicked()
{
    if (m_downloadItem) {
        m_downloadItem->setStatus("cancelled");
    }
}

void ProgressWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_filenameLabel = new QLabel();
    layout->addWidget(m_filenameLabel);

    m_statusLabel = new QLabel();
    layout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar();
    layout->addWidget(m_progressBar);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_pauseButton = new QPushButton("Pause");
    connect(m_pauseButton, &QPushButton::clicked, this, &ProgressWidget::onPauseClicked);
    buttonLayout->addWidget(m_pauseButton);

    m_resumeButton = new QPushButton("Resume");
    connect(m_resumeButton, &QPushButton::clicked, this, &ProgressWidget::onResumeClicked);
    buttonLayout->addWidget(m_resumeButton);

    m_cancelButton = new QPushButton("Cancel");
    connect(m_cancelButton, &QPushButton::clicked, this, &ProgressWidget::onCancelClicked);
    buttonLayout->addWidget(m_cancelButton);

    layout->addLayout(buttonLayout);

    m_speedLabel = new QLabel();
    layout->addWidget(m_speedLabel);

    m_etaLabel = new QLabel();
    layout->addWidget(m_etaLabel);
}

QString ProgressWidget::formatSize(qint64 bytes) const
{
    if (bytes < 1024) return QString("%1 B").arg(bytes);
    else if (bytes < 1024 * 1024) return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    else if (bytes < 1024 * 1024 * 1024) return QString("%1 MB").arg(bytes / (1024.0 * 1024), 0, 'f', 1);
    else return QString("%1 GB").arg(bytes / (1024.0 * 1024 * 1024), 0, 'f', 1);
}

QString ProgressWidget::formatTime(int seconds) const
{
    if (seconds <= 0) return "--";
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}