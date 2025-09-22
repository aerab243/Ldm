#include "MediaPreviewWidget.h"
#include <QUrl>
#include <QMessageBox>

MediaPreviewWidget::MediaPreviewWidget(QWidget *parent)
    : QWidget(parent)
{
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoWidget = new QVideoWidget(this);
    m_audioOutput = new QAudioOutput(this);
    m_layout = new QVBoxLayout(this);

    m_mediaPlayer->setVideoOutput(m_videoWidget);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    m_layout->addWidget(m_videoWidget);
    setLayout(m_layout);

    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MediaPreviewWidget::onMediaStatusChanged);
    connect(m_mediaPlayer, QOverload<QMediaPlayer::Error, const QString &>::of(&QMediaPlayer::errorOccurred), this, &MediaPreviewWidget::onErrorOccurred);
}

MediaPreviewWidget::~MediaPreviewWidget()
{
    // Qt handles deletion
}

void MediaPreviewWidget::setMedia(const QString &filePath)
{
    m_mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
}

void MediaPreviewWidget::play()
{
    m_mediaPlayer->play();
}

void MediaPreviewWidget::pause()
{
    m_mediaPlayer->pause();
}

void MediaPreviewWidget::stop()
{
    m_mediaPlayer->stop();
}

void MediaPreviewWidget::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    // Handle status changes if needed
    if (status == QMediaPlayer::LoadedMedia) {
        // Media loaded successfully
    }
}

void MediaPreviewWidget::onErrorOccurred(QMediaPlayer::Error error, const QString &errorString)
{
    QMessageBox::warning(this, "Media Error", QString("Error playing media: %1").arg(errorString));
}