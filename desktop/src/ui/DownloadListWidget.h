#ifndef DOWNLOADLISTWIDGET_H
#define DOWNLOADLISTWIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "core/DownloadItem.h"

class DownloadListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit DownloadListWidget(QWidget *parent = nullptr);
    ~DownloadListWidget();

    void addDownload(const DownloadItem &item);
    void updateDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal);
    void updateDownloadStatus(int downloadId, const QString &status);
    void removeDownload(int downloadId);

signals:
    void downloadPaused(int downloadId);
    void downloadResumed(int downloadId);
    void downloadCancelled(int downloadId);
    void downloadSelected(int downloadId);

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onPauseClicked();
    void onResumeClicked();
    void onCancelClicked();

private:
    struct DownloadWidget {
        QWidget *widget;
        QLabel *nameLabel;
        QLabel *sizeLabel;
        QProgressBar *progressBar;
        QLabel *statusLabel;
        QPushButton *pauseButton;
        QPushButton *resumeButton;
        QPushButton *cancelButton;
        int downloadId;
    };

    QList<DownloadWidget> m_downloadWidgets;

    QWidget* createDownloadWidget(const DownloadItem &item);
    DownloadWidget* findDownloadWidget(int downloadId);
};

#endif // DOWNLOADLISTWIDGET_H