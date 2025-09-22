#ifndef MEDIAPREVIEWWIDGET_H
#define MEDIAPREVIEWWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QVBoxLayout>

class MediaPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPreviewWidget(QWidget *parent = nullptr);
    ~MediaPreviewWidget();

    void setMedia(const QString &filePath);
    void play();
    void pause();
    void stop();

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onErrorOccurred(QMediaPlayer::Error error, const QString &errorString);

private:
    QMediaPlayer *m_mediaPlayer;
    QVideoWidget *m_videoWidget;
    QAudioOutput *m_audioOutput;
    QVBoxLayout *m_layout;
};

#endif // MEDIAPREVIEWWIDGET_H