#include "DownloadItem.h"

DownloadItem::DownloadItem(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_totalSize(-1)
    , m_downloadedSize(0)
    , m_speed(0)
    , m_eta(0)
    , m_categoryId(0)
    , m_priority(1)
    , m_segments(1)
    , m_resumeSupported(true)
    , m_antivirusScanned(false)
    , m_encrypted(false)
{
    m_createdAt = QDateTime::currentDateTime();
}

DownloadItem::DownloadItem(int id, const QString &url, const QString &filename, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_url(url)
    , m_filename(filename)
    , m_status("queued")
    , m_totalSize(-1)
    , m_downloadedSize(0)
    , m_speed(0)
    , m_eta(0)
    , m_categoryId(0)
    , m_priority(1)
    , m_segments(1)
    , m_resumeSupported(true)
    , m_antivirusScanned(false)
    , m_encrypted(false)
{
    m_createdAt = QDateTime::currentDateTime();
}

double DownloadItem::getProgress() const
{
    if (m_totalSize <= 0) {
        return 0.0;
    }
    return static_cast<double>(m_downloadedSize) / static_cast<double>(m_totalSize);
}