#ifndef DOWNLOADQUEUE_H
#define DOWNLOADQUEUE_H

#include <QObject>
#include <QQueue>
#include <QList>
#include <QTimer>
#include "DownloadItem.h"

class DownloadQueue : public QObject
{
    Q_OBJECT

public:
    explicit DownloadQueue(QObject *parent = nullptr);
    ~DownloadQueue();

    void addDownload(DownloadItem *item);
    void removeDownload(int id);
    void pauseDownload(int id);
    void resumeDownload(int id);
    void cancelDownload(int id);

    void setMaxConcurrentDownloads(int max);
    int getMaxConcurrentDownloads() const;

    void setBandwidthLimit(qint64 bytesPerSecond);
    qint64 getBandwidthLimit() const;

    QList<DownloadItem*> getActiveDownloads() const;
    QList<DownloadItem*> getQueuedDownloads() const;

signals:
    void downloadStarted(DownloadItem *item);
    void downloadCompleted(DownloadItem *item);
    void downloadFailed(DownloadItem *item, const QString &error);
    void queueUpdated();

private slots:
    void onDownloadCompleted();
    void onDownloadFailed(const QString &error);
    void checkQueue();

private:
    QList<DownloadItem*> m_queue;
    QList<DownloadItem*> m_activeDownloads;
    int m_maxConcurrentDownloads;
    qint64 m_bandwidthLimit;
    QTimer *m_queueTimer;

    void sortQueueByPriority();
    void startNextDownloads();
    bool canStartDownload() const;
};

#endif // DOWNLOADQUEUE_H