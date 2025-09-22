#include "DownloadQueue.h"
#include <QDebug>
#include <algorithm>

DownloadQueue::DownloadQueue(QObject *parent)
    : QObject(parent)
    , m_maxConcurrentDownloads(3)
    , m_bandwidthLimit(0) // 0 means no limit
    , m_queueTimer(new QTimer(this))
{
    m_queueTimer->setSingleShot(true);
    connect(m_queueTimer, &QTimer::timeout, this, &DownloadQueue::checkQueue);
}

DownloadQueue::~DownloadQueue()
{
}

void DownloadQueue::addDownload(DownloadItem *item)
{
    if (!item) {
        return;
    }

    // Connect signals
    connect(item, &DownloadItem::downloadCompleted, this, &DownloadQueue::onDownloadCompleted);
    connect(item, &DownloadItem::statusChanged, this, [this, item](const QString &status) {
        if (status == "failed") {
            onDownloadFailed(item, item->getErrorMessage());
        }
    });

    m_queue.append(item);
    sortQueueByPriority();
    checkQueue();
    emit queueUpdated();
}

void DownloadQueue::removeDownload(int id)
{
    // Remove from queue
    for (int i = 0; i < m_queue.size(); ++i) {
        if (m_queue[i]->getId() == id) {
            m_queue.removeAt(i);
            emit queueUpdated();
            return;
        }
    }

    // Remove from active
    for (int i = 0; i < m_activeDownloads.size(); ++i) {
        if (m_activeDownloads[i]->getId() == id) {
            m_activeDownloads[i]->setStatus("cancelled");
            m_activeDownloads.removeAt(i);
            checkQueue();
            emit queueUpdated();
            return;
        }
    }
}

void DownloadQueue::pauseDownload(int id)
{
    for (auto *item : m_activeDownloads) {
        if (item->getId() == id) {
            item->setStatus("paused");
            m_activeDownloads.removeOne(item);
            m_queue.append(item);
            sortQueueByPriority();
            checkQueue();
            emit queueUpdated();
            return;
        }
    }
}

void DownloadQueue::resumeDownload(int id)
{
    for (auto *item : m_queue) {
        if (item->getId() == id) {
            if (canStartDownload()) {
                m_queue.removeOne(item);
                m_activeDownloads.append(item);
                item->setStatus("downloading");
                emit downloadStarted(item);
                emit queueUpdated();
            }
            return;
        }
    }
}

void DownloadQueue::cancelDownload(int id)
{
    removeDownload(id);
}

void DownloadQueue::setMaxConcurrentDownloads(int max)
{
    m_maxConcurrentDownloads = max;
    checkQueue();
}

int DownloadQueue::getMaxConcurrentDownloads() const
{
    return m_maxConcurrentDownloads;
}

void DownloadQueue::setBandwidthLimit(qint64 bytesPerSecond)
{
    m_bandwidthLimit = bytesPerSecond;
    // TODO: Distribute limit among active downloads
}

qint64 DownloadQueue::getBandwidthLimit() const
{
    return m_bandwidthLimit;
}

QList<DownloadItem*> DownloadQueue::getActiveDownloads() const
{
    return m_activeDownloads;
}

QList<DownloadItem*> DownloadQueue::getQueuedDownloads() const
{
    return m_queue;
}

void DownloadQueue::onDownloadCompleted()
{
    DownloadItem *item = qobject_cast<DownloadItem*>(sender());
    if (item) {
        m_activeDownloads.removeOne(item);
        emit downloadCompleted(item);
        checkQueue();
        emit queueUpdated();
    }
}

void DownloadQueue::onDownloadFailed(const QString &error)
{
    DownloadItem *item = qobject_cast<DownloadItem*>(sender());
    if (item) {
        m_activeDownloads.removeOne(item);
        emit downloadFailed(item, error);
        checkQueue();
        emit queueUpdated();
    }
}

void DownloadQueue::checkQueue()
{
    startNextDownloads();
}

void DownloadQueue::sortQueueByPriority()
{
    std::sort(m_queue.begin(), m_queue.end(), [](DownloadItem *a, DownloadItem *b) {
        return a->getPriority() > b->getPriority(); // Higher priority first
    });
}

void DownloadQueue::startNextDownloads()
{
    while (!m_queue.isEmpty() && canStartDownload()) {
        DownloadItem *item = m_queue.takeFirst();
        m_activeDownloads.append(item);
        item->setStatus("downloading");
        emit downloadStarted(item);
    }
}

bool DownloadQueue::canStartDownload() const
{
    return m_activeDownloads.size() < m_maxConcurrentDownloads;
}