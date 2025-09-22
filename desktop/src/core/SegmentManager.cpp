#include "SegmentManager.h"
#include <QDir>
#include <QDebug>

SegmentManager::SegmentManager(const QUrl &url, const QString &filepath, int numSegments, QObject *parent)
    : QObject(parent)
    , m_url(url)
    , m_filepath(filepath)
    , m_numSegments(numSegments)
    , m_totalSize(-1)
    , m_isDownloading(false)
    , m_isPaused(false)
    , m_isCompleted(false)
    , m_hasFailed(false)
    , m_sizeFetcher(new NetworkManager(this))
{
}

SegmentManager::~SegmentManager()
{
    cancelDownload();
    if (m_file.isOpen()) {
        m_file.close();
    }
}

void SegmentManager::initializeSegments(qint64 totalSize)
{
    m_segments.clear();
    qint64 segmentSize = totalSize / m_numSegments;
    qint64 remainder = totalSize % m_numSegments;
    qint64 start = 0;
    for (int i = 0; i < m_numSegments; ++i) {
        qint64 end = start + segmentSize - 1;
        if (i < remainder) end++;
        DownloadSegment segment;
        segment.index = i;
        segment.startOffset = start;
        segment.endOffset = (i == m_numSegments - 1) ? -1 : end;
        segment.downloadedSize = 0;
        segment.status = "pending";
        segment.networkManager = new NetworkManager(this);
        connect(segment.networkManager, &NetworkManager::downloadProgress,
                this, &SegmentManager::onNetworkProgress);
        connect(segment.networkManager, &NetworkManager::downloadFinished,
                this, &SegmentManager::onNetworkFinished);
        m_segments.append(segment);
        start = end + 1;
    }
}

void SegmentManager::startDownload()
{
    if (m_isDownloading || m_isCompleted || m_hasFailed) {
        return;
    }

    m_isDownloading = true;
    m_isPaused = false;

    if (m_totalSize == -1) {
        fetchTotalSize();
    } else {
        initializeSegments(m_totalSize);

        // Use Qt Concurrent to start all segments in parallel
        QList<int> segmentIndices;
        for (int i = 0; i < m_numSegments; ++i) {
            segmentIndices.append(i);
        }

        // Run segment starts concurrently
        QFuture<void> future = QtConcurrent::map(segmentIndices, [this](int index) {
            this->startSegment(index);
        });

        QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);
        watcher->setFuture(future);
        m_segmentWatchers.append(watcher);
    }
}

void SegmentManager::startSegment(int index)
{
    if (index < 0 || index >= m_segments.size()) {
        return;
    }

    DownloadSegment &segment = m_segments[index];
    if (segment.status != "pending") {
        return;
    }

    segment.status = "downloading";
    QString partFile = m_filepath + ".part" + QString::number(index);
    segment.networkManager->downloadRange(m_url, partFile, segment.startOffset, segment.endOffset);
}

void SegmentManager::pauseDownload()
{
    if (!m_isDownloading || m_isPaused) {
        return;
    }

    m_isPaused = true;

    // Cancel any pending concurrent operations
    for (auto watcher : m_segmentWatchers) {
        watcher->cancel();
    }

    for (auto &segment : m_segments) {
        if (segment.status == "downloading") {
            segment.networkManager->pauseDownload();
            segment.status = "paused";
        }
    }
}

void SegmentManager::resumeDownload()
{
    if (!m_isPaused) {
        return;
    }

    m_isPaused = false;
    for (auto &segment : m_segments) {
        if (segment.status == "paused") {
            segment.status = "downloading";
            segment.networkManager->resumeDownload();
        }
    }
}

void SegmentManager::cancelDownload()
{
    m_isDownloading = false;
    m_isPaused = false;

    // Cancel all concurrent operations
    for (auto watcher : m_segmentWatchers) {
        watcher->cancel();
        watcher->waitForFinished();
    }
    m_segmentWatchers.clear();

    for (auto &segment : m_segments) {
        segment.networkManager->cancelDownload();
        segment.status = "cancelled";
    }
    // Clean up part files
    for (int i = 0; i < m_segments.size(); ++i) {
        QString partFile = m_filepath + ".part" + QString::number(i);
        QFile::remove(partFile);
    }
}

qint64 SegmentManager::getTotalDownloaded() const
{
    qint64 total = 0;
    for (const auto &segment : m_segments) {
        total += segment.downloadedSize;
    }
    return total;
}

qint64 SegmentManager::getTotalSize() const
{
    return m_totalSize;
}

bool SegmentManager::isCompleted() const
{
    return m_isCompleted;
}

bool SegmentManager::hasFailed() const
{
    return m_hasFailed;
}

void SegmentManager::onNetworkProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    // Find which segment this is from
    NetworkManager *sender = qobject_cast<NetworkManager*>(QObject::sender());
    if (!sender) {
        return;
    }

    for (int i = 0; i < m_segments.size(); ++i) {
        if (m_segments[i].networkManager == sender) {
            m_segments[i].downloadedSize = bytesReceived;
            m_totalSize = bytesTotal;
            emit segmentProgress(i, bytesReceived, bytesTotal);
            break;
        }
    }
}

void SegmentManager::onNetworkFinished(bool success, const QString &errorMessage)
{
    NetworkManager *sender = qobject_cast<NetworkManager*>(QObject::sender());
    if (!sender) {
        return;
    }

    for (int i = 0; i < m_segments.size(); ++i) {
        if (m_segments[i].networkManager == sender) {
            if (success) {
                m_segments[i].status = "completed";
                emit segmentCompleted(i);
            } else {
                m_segments[i].status = "failed";
                emit segmentFailed(i, errorMessage);
                m_hasFailed = true;
                emit downloadFailed(errorMessage);
                return;
            }
            break;
        }
    }

    // Check if all segments are completed
    bool allCompleted = true;
    for (const auto &segment : m_segments) {
        if (segment.status != "completed") {
            allCompleted = false;
            break;
        }
    }

    if (allCompleted) {
        m_isCompleted = true;
        mergeSegments();
        emit allSegmentsCompleted();
    }
}

void SegmentManager::mergeSegments()
{
    QFile finalFile(m_filepath);
    if (!finalFile.open(QIODevice::WriteOnly)) {
        m_hasFailed = true;
        emit downloadFailed("Failed to open final file for writing");
        return;
    }
    for (int i = 0; i < m_segments.size(); ++i) {
        QString partFile = m_filepath + ".part" + QString::number(i);
        QFile part(partFile);
        if (!part.open(QIODevice::ReadOnly)) {
            m_hasFailed = true;
            emit downloadFailed("Failed to open part file");
            finalFile.close();
            return;
        }
        finalFile.write(part.readAll());
        part.close();
        part.remove();
    }
    finalFile.close();
}

void SegmentManager::fetchTotalSize()
{
    m_totalSize = m_sizeFetcher->getContentLength(m_url);
    if (m_totalSize > 0) {
        onTotalSizeFetched(m_totalSize);
    } else {
        m_hasFailed = true;
        emit downloadFailed("Failed to get content length");
    }
}

void SegmentManager::onTotalSizeFetched(qint64 size)
{
    if (size <= 0) {
        m_hasFailed = true;
        emit downloadFailed("Unable to get file size");
        return;
    }
    m_totalSize = size;
    initializeSegments(m_totalSize);
    for (int i = 0; i < m_numSegments; ++i) {
        startSegment(i);
    }
}

bool SegmentManager::supportsResume()
{
    // Check if server supports resume
    // For basic implementation, assume yes
    return true;
}