#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include "core/DownloadItem.h"

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWidget(DownloadItem *item, QWidget *parent = nullptr);
    ~ProgressWidget();

    void updateProgress();

private slots:
    void onPauseClicked();
    void onResumeClicked();
    void onCancelClicked();

private:
    DownloadItem *m_downloadItem;

    QLabel *m_filenameLabel;
    QLabel *m_statusLabel;
    QLabel *m_speedLabel;
    QLabel *m_etaLabel;
    QProgressBar *m_progressBar;
    QPushButton *m_pauseButton;
    QPushButton *m_resumeButton;
    QPushButton *m_cancelButton;

    void setupUI();
    QString formatSize(qint64 bytes) const;
    QString formatTime(int seconds) const;
};

#endif // PROGRESSWIDGET_H