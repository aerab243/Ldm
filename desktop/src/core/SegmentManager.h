#ifndef SEGMENTMANAGER_H
#define SEGMENTMANAGER_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QList>
#include <QNetworkReply>
#include <QFile>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include "NetworkManager.h"

struct DownloadSegment {
    int index;
    qint64 startOffset;
    qint64 endOffset;
    qint64 downloadedSize;
    QString status; // "pending", "downloading", "completed", "failed"
    NetworkManager *networkManager;
};

class SegmentManager : public QObject
{
    Q_OBJECT

public:
    explicit SegmentManager(const QUrl &url, const QString &filepath, int numSegments, QObject *parent = nullptr);
    ~SegmentManager();

    void startDownload();
    void pauseDownload();
    void resumeDownload();
    void cancelDownload();
    void fetchTotalSize();

    qint64 getTotalDownloaded() const;
    qint64 getTotalSize() const;
    bool isCompleted() const;
    bool hasFailed() const;

signals:
    void segmentProgress(int segmentIndex, qint64 bytesReceived, qint64 bytesTotal);
    void segmentCompleted(int segmentIndex);
    void segmentFailed(int segmentIndex, const QString &error);
    void allSegmentsCompleted();
    void downloadFailed(const QString &error);
    void totalSizeFetched(qint64 size);

private slots:
    void onNetworkProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onNetworkFinished(bool success, const QString &errorMessage);
    void onTotalSizeFetched(qint64 size);

private:
    QUrl m_url;
    QString m_filepath;
    int m_numSegments;
    QList<DownloadSegment> m_segments;
    QFile m_file;
    qint64 m_totalSize;
    bool m_isDownloading;
    bool m_isPaused;
    bool m_isCompleted;
    bool m_hasFailed;
    NetworkManager *m_sizeFetcher;
    QList<QFutureWatcher<void>*> m_segmentWatchers;

    void initializeSegments(qint64 totalSize);
    void startSegment(int index);
    void mergeSegments();
    bool supportsResume();
};

#endif // SEGMENTMANAGER_H