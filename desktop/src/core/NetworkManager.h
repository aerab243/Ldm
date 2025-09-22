#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QTimer>
#include "utils/MemoryMappedFile.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    // Download methods
    bool downloadFile(const QUrl &url, const QString &filepath);
    bool downloadRange(const QUrl &url, const QString &filepath, qint64 startOffset, qint64 endOffset);
    bool supportsResume(const QUrl &url);
    qint64 getContentLength(const QUrl &url);

    // Retry configuration
    void setMaxRetries(int retries);
    int getMaxRetries() const;

    // Configuration
    void setProxy(const QNetworkProxy &proxy);
    QNetworkProxy getProxy() const;
    void setAuthentication(const QString &username, const QString &password);
    void setUserAgent(const QString &userAgent);
    void setTimeout(int seconds);

    // Status
    bool isDownloading() const;
    qint64 getDownloadedBytes() const;
    qint64 getTotalBytes() const;

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(bool success, const QString &errorMessage = QString());
    void downloadStarted();

public slots:
    void startDownload();
    void pauseDownload();
    void resumeDownload();
    void cancelDownload();

private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onReadyRead();
    void onFinished();
    void retryDownload();

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QString m_filepath;
    QNetworkProxy m_proxy;
    QString m_username;
    QString m_password;
    QString m_userAgent;
    int m_timeout;
    bool m_isDownloading;
    qint64 m_downloadedBytes;
    qint64 m_totalBytes;
    QString m_url;
    qint64 m_startOffset;
    qint64 m_endOffset;
    int m_maxRetries;
    int m_currentRetry;
    QTimer *m_retryTimer;
    MemoryMappedFile *m_mappedFile;
};

#endif // NETWORKMANAGER_H