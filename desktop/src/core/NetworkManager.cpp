#include "NetworkManager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_timeout(30)
    , m_isDownloading(false)
    , m_downloadedBytes(0)
    , m_totalBytes(0)
    , m_url("")
    , m_startOffset(0)
    , m_endOffset(-1)
    , m_maxRetries(3)
    , m_currentRetry(0)
    , m_retryTimer(new QTimer(this))
    , m_mappedFile(new MemoryMappedFile(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &NetworkManager::onFinished);
    connect(m_retryTimer, &QTimer::timeout, this, &NetworkManager::retryDownload);
}

NetworkManager::~NetworkManager()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

bool NetworkManager::downloadFile(const QUrl &url, const QString &filepath)
{
    if (m_isDownloading) {
        return false;
    }

    m_filepath = filepath;
    m_url = url.toString();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, m_userAgent);

    if (!m_username.isEmpty() && !m_password.isEmpty()) {
        QString auth = QString("%1:%2").arg(m_username, m_password);
        QByteArray authData = auth.toUtf8().toBase64();
        request.setRawHeader("Authorization", "Basic " + authData);
    }

    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &NetworkManager::onDownloadProgress);
    connect(m_currentReply, &QNetworkReply::readyRead, this, &NetworkManager::onReadyRead);

    m_isDownloading = true;
    emit downloadStarted();
    return true;
}

bool NetworkManager::downloadRange(const QUrl &url, const QString &filepath, qint64 startOffset, qint64 endOffset)
{
    if (m_isDownloading) {
        return false;
    }

    m_filepath = filepath;
    m_url = url.toString();
    m_startOffset = startOffset;
    m_endOffset = endOffset;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, m_userAgent);
    QString range = QString("bytes=%1-%2").arg(startOffset).arg(endOffset > 0 ? QString::number(endOffset) : "");
    request.setRawHeader("Range", range.toUtf8());

    if (!m_username.isEmpty() && !m_password.isEmpty()) {
        QString auth = QString("%1:%2").arg(m_username, m_password);
        QByteArray authData = auth.toUtf8().toBase64();
        request.setRawHeader("Authorization", "Basic " + authData);
    }

    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &NetworkManager::onDownloadProgress);
    connect(m_currentReply, &QNetworkReply::readyRead, this, &NetworkManager::onReadyRead);

    m_isDownloading = true;
    emit downloadStarted();
    return true;
}

qint64 NetworkManager::getContentLength(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, m_userAgent);
    QNetworkReply *reply = m_networkManager->head(request);

    // Wait for response or timeout
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(m_timeout * 1000);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    qint64 contentLength = -1;
    if (timer.isActive()) {
        timer.stop();
        contentLength = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    }
    reply->deleteLater();
    return contentLength;
}

void NetworkManager::setMaxRetries(int retries)
{
    m_maxRetries = retries;
}

int NetworkManager::getMaxRetries() const
{
    return m_maxRetries;
}

bool NetworkManager::supportsResume(const QUrl &url)
{
    // Simplified: assume resume is supported
    return true;
}



void NetworkManager::setProxy(const QNetworkProxy &proxy)
{
    m_proxy = proxy;
    m_networkManager->setProxy(proxy);
}

QNetworkProxy NetworkManager::getProxy() const
{
    return m_proxy;
}

void NetworkManager::setAuthentication(const QString &username, const QString &password)
{
    m_username = username;
    m_password = password;
}

void NetworkManager::setUserAgent(const QString &userAgent)
{
    m_userAgent = userAgent;
}

void NetworkManager::setTimeout(int seconds)
{
    m_timeout = seconds;
}

bool NetworkManager::isDownloading() const
{
    return m_isDownloading;
}

qint64 NetworkManager::getDownloadedBytes() const
{
    return m_downloadedBytes;
}

qint64 NetworkManager::getTotalBytes() const
{
    return m_totalBytes;
}

void NetworkManager::startDownload()
{
    // Implementation for starting download if not already started
}

void NetworkManager::pauseDownload()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_isDownloading = false;
    }
}

void NetworkManager::resumeDownload()
{
    // Implementation for resume - would need to store offset
}

void NetworkManager::cancelDownload()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_isDownloading = false;
    }
}

void NetworkManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_downloadedBytes = bytesReceived;
    m_totalBytes = bytesTotal;
    emit downloadProgress(bytesReceived, bytesTotal);
}

void NetworkManager::onReadyRead()
{
    if (!m_currentReply) {
        return;
    }

    QByteArray data = m_currentReply->readAll();
    if (!data.isEmpty()) {
        // For large files, use memory mapping
        if (m_totalBytes > 10 * 1024 * 1024) { // 10MB threshold
            if (!m_mappedFile->isOpen()) {
                if (!m_mappedFile->open(m_filepath, QFile::WriteOnly)) {
                    emit downloadFinished(false, "Failed to open memory mapped file");
                    return;
                }
            }
            // Write to memory mapped file
            // This is simplified - in reality, need to handle offsets
            m_mappedFile->resize(m_totalBytes);
            if (m_mappedFile->map()) {
                // Copy data to mapped memory
                memcpy((void*)(m_mappedFile->data() + m_downloadedBytes - data.size()), data.constData(), data.size());
                m_mappedFile->flush();
            }
        } else {
            // For small files, use regular QFile
            QFile file(m_filepath);
            QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Append;
            if (m_startOffset == 0 && !file.exists()) {
                mode = QIODevice::WriteOnly;
            }
            if (file.open(mode)) {
                file.write(data);
                file.close();
            }
        }
    }
}

void NetworkManager::onFinished()
{
    if (!m_currentReply) {
        return;
    }

    bool success = false;
    QString errorMessage;

    if (m_currentReply->error() == QNetworkReply::NoError) {
        // Check if data was already written via readyRead
        if (m_mappedFile->isOpen()) {
            // Data was written via memory mapping
            m_mappedFile->close();
            success = true;
        } else {
            // Fallback: save remaining data to file
            QFile file(m_filepath);
            QIODevice::OpenMode mode = QIODevice::WriteOnly;
            if (m_startOffset > 0) {
                mode |= QIODevice::Append;
            }
            if (file.open(mode)) {
                file.write(m_currentReply->readAll());
                file.close();
                success = true;
            } else {
                errorMessage = "Failed to save file";
            }
        }
    } else {
        errorMessage = m_currentReply->errorString();
    }

    m_currentReply->deleteLater();
    m_currentReply = nullptr;

    if (!success && m_currentRetry < m_maxRetries) {
        m_currentRetry++;
        m_retryTimer->start(1000 * m_currentRetry); // Exponential backoff
        return;
    }

    m_isDownloading = false;
    m_currentRetry = 0;
    emit downloadFinished(success, errorMessage);
}

void NetworkManager::retryDownload()
{
    // Retry the download
    if (m_startOffset > 0 || m_endOffset >= 0) {
        downloadRange(QUrl(m_url), m_filepath, m_startOffset, m_endOffset);
    } else {
        downloadFile(QUrl(m_url), m_filepath);
    }
}