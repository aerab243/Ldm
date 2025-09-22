#include "DownloadEngine.h"
#include <QDebug>
#include <QDir>

DownloadEngine::DownloadEngine(QObject *parent)
    : QObject(parent)
    , m_threadPool(new QThreadPool(this))
    , m_maxConcurrentDownloads(3)
    , m_maxSegmentsPerDownload(4) // Increased for better parallel processing
{
    m_threadPool->setMaxThreadCount(m_maxConcurrentDownloads);
}

DownloadEngine::~DownloadEngine()
{
    stopAllDownloads();
}

bool DownloadEngine::startDownload(DownloadItem *item)
{
    if (!item || m_downloads.contains(item->getId())) {
        return false;
    }

    // Create segment manager for this download
    SegmentManager *segmentManager = new SegmentManager(
        QUrl(item->getUrl()),
        item->getFilepath(),
        m_maxSegmentsPerDownload,
        this
    );

    // Connect signals
    connect(segmentManager, &SegmentManager::segmentProgress,
            this, &DownloadEngine::onSegmentProgress);
    connect(segmentManager, &SegmentManager::segmentCompleted,
            this, &DownloadEngine::onSegmentCompleted);
    connect(segmentManager, &SegmentManager::segmentFailed,
            this, &DownloadEngine::onSegmentFailed);
    connect(segmentManager, &SegmentManager::allSegmentsCompleted,
            [this, item]() {
                emit downloadCompleted(item->getId());
            });
    connect(segmentManager, &SegmentManager::downloadFailed,
            [this, item](const QString &error) {
                emit downloadFailed(item->getId(), error);
            });

    m_downloads[item->getId()] = item;
    m_segmentManagers[item->getId()] = segmentManager;

    // Use Qt Concurrent for asynchronous download start
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, [this, item, watcher]() {
        m_downloadWatchers.remove(item->getId());
        watcher->deleteLater();
    });

    m_downloadWatchers[item->getId()] = watcher;

    // Start download asynchronously using Qt Concurrent
    QFuture<void> future = QtConcurrent::run([segmentManager]() {
        segmentManager->startDownload();
    });

    watcher->setFuture(future);
    emit downloadStarted(item->getId());

    return true;
}

void DownloadEngine::pauseDownload(int downloadId)
{
    if (m_segmentManagers.contains(downloadId)) {
        m_segmentManagers[downloadId]->pauseDownload();
        emit downloadPaused(downloadId);
    }

    // Cancel any pending concurrent operations
    if (m_downloadWatchers.contains(downloadId)) {
        m_downloadWatchers[downloadId]->cancel();
    }
}

void DownloadEngine::resumeDownload(int downloadId)
{
    if (m_segmentManagers.contains(downloadId)) {
        m_segmentManagers[downloadId]->resumeDownload();
        emit downloadResumed(downloadId);
    }
}

void DownloadEngine::cancelDownload(int downloadId)
{
    // Cancel concurrent operations first
    if (m_downloadWatchers.contains(downloadId)) {
        m_downloadWatchers[downloadId]->cancel();
        m_downloadWatchers[downloadId]->waitForFinished();
        m_downloadWatchers.remove(downloadId);
    }

    if (m_segmentManagers.contains(downloadId)) {
        m_segmentManagers[downloadId]->cancelDownload();
        cleanupDownload(downloadId);
        emit downloadCancelled(downloadId);
    }
}

void DownloadEngine::stopAllDownloads()
{
    // Cancel all concurrent operations first
    for (auto watcher : m_downloadWatchers) {
        watcher->cancel();
        watcher->waitForFinished();
    }
    m_downloadWatchers.clear();

    // Stop all segment managers
    for (auto it = m_segmentManagers.begin(); it != m_segmentManagers.end(); ++it) {
        it.value()->cancelDownload();
    }
    m_downloads.clear();
    m_segmentManagers.clear();
    m_networkManagers.clear();
}

void DownloadEngine::setMaxConcurrentDownloads(int max)
{
    m_maxConcurrentDownloads = max;
    m_threadPool->setMaxThreadCount(max);
}

void DownloadEngine::setMaxSegmentsPerDownload(int max)
{
    m_maxSegmentsPerDownload = max;
}

int DownloadEngine::getMaxConcurrentDownloads() const
{
    return m_maxConcurrentDownloads;
}

int DownloadEngine::getMaxSegmentsPerDownload() const
{
    return m_maxSegmentsPerDownload;
}

QList<DownloadItem*> DownloadEngine::getActiveDownloads() const
{
    return m_downloads.values();
}

DownloadItem* DownloadEngine::getDownload(int id) const
{
    return m_downloads.value(id, nullptr);
}

bool DownloadEngine::isDownloading(int id) const
{
    return m_segmentManagers.contains(id) && !m_segmentManagers[id]->isCompleted();
}

void DownloadEngine::onSegmentProgress(int segmentIndex, qint64 bytesReceived, qint64 bytesTotal)
{
    // Find which download this belongs to
    SegmentManager *sender = qobject_cast<SegmentManager*>(QObject::sender());
    if (!sender) {
        return;
    }

    for (auto it = m_segmentManagers.begin(); it != m_segmentManagers.end(); ++it) {
        if (it.value() == sender) {
            emit downloadProgress(it.key(), bytesReceived, bytesTotal);
            break;
        }
    }
}

void DownloadEngine::onSegmentCompleted(int segmentIndex)
{
    // Segment completed - handled by allSegmentsCompleted signal
}

void DownloadEngine::onSegmentFailed(int segmentIndex, const QString &error)
{
    // Segment failed - handled by downloadFailed signal
}

void DownloadEngine::cleanupDownload(int downloadId)
{
    if (m_segmentManagers.contains(downloadId)) {
        delete m_segmentManagers[downloadId];
        m_segmentManagers.remove(downloadId);
    }
    m_downloads.remove(downloadId);
    m_networkManagers.remove(downloadId);
    m_downloadWatchers.remove(downloadId);
}

void DownloadEngine::startDownloadSegments(DownloadItem *item)
{
    // For basic implementation, handled in startDownload
}

void DownloadEngine::updateDownloadProgress(int downloadId)
{
    // For basic implementation, progress is updated via signals
}