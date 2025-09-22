#ifndef DOWNLOADENGINE_H
#define DOWNLOADENGINE_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QThreadPool>
#include <QMutex>
#include <QWaitCondition>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include "DownloadItem.h"
#include "NetworkManager.h"
#include "SegmentManager.h"

class DownloadEngine : public QObject
{
    Q_OBJECT

public:
    explicit DownloadEngine(QObject *parent = nullptr);
    ~DownloadEngine();

    // Download management
    bool startDownload(DownloadItem *item);
    void pauseDownload(int downloadId);
    void resumeDownload(int downloadId);
    void cancelDownload(int downloadId);
    void stopAllDownloads();

    // Configuration
    void setMaxConcurrentDownloads(int max);
    void setMaxSegmentsPerDownload(int max);
    int getMaxConcurrentDownloads() const;
    int getMaxSegmentsPerDownload() const;

    // Status
    QList<DownloadItem*> getActiveDownloads() const;
    DownloadItem* getDownload(int id) const;
    bool isDownloading(int id) const;

signals:
    void downloadStarted(int downloadId);
    void downloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal);
    void downloadCompleted(int downloadId);
    void downloadFailed(int downloadId, const QString &error);
    void downloadPaused(int downloadId);
    void downloadResumed(int downloadId);
    void downloadCancelled(int downloadId);

private slots:
    void onSegmentProgress(int segmentIndex, qint64 bytesReceived, qint64 bytesTotal);
    void onSegmentCompleted(int segmentIndex);
    void onSegmentFailed(int segmentIndex, const QString &error);

private:
    QThreadPool *m_threadPool;
    QHash<int, DownloadItem*> m_downloads;
    QHash<int, SegmentManager*> m_segmentManagers;
    QHash<int, QList<NetworkManager*>> m_networkManagers;
    QHash<int, QFutureWatcher<void>*> m_downloadWatchers;
    QMutex m_mutex;
    QWaitCondition m_waitCondition;
    int m_maxConcurrentDownloads;
    int m_maxSegmentsPerDownload;

    void cleanupDownload(int downloadId);
    void startDownloadSegments(DownloadItem *item);
    void updateDownloadProgress(int downloadId);
};

#endif // DOWNLOADENGINE_H