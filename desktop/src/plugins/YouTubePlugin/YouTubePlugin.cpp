#include "YouTubePlugin.h"
#include <QUrl>
#include <QUrlQuery>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

YouTubePlugin::YouTubePlugin(QObject *parent)
    : PluginInterface(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
{
}

YouTubePlugin::~YouTubePlugin()
{
    if (m_currentReply) {
        m_currentReply->abort();
    }
}

bool YouTubePlugin::initialize()
{
    // Initialize any resources needed
    return true;
}

bool YouTubePlugin::shutdown()
{
    // Clean up resources
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply = nullptr;
    }
    return true;
}

bool YouTubePlugin::canHandleUrl(const QString &url) const
{
    QString videoId;
    return parseYouTubeUrl(url, videoId);
}

QVariantMap YouTubePlugin::getDownloadInfo(const QString &url)
{
    m_downloadInfo.clear();
    m_currentUrl = url;
    
    if (!canHandleUrl(url)) {
        emit pluginError("Invalid YouTube URL");
        return m_downloadInfo;
    }
    
    // For this implementation, we'll use a simplified approach
    // In a real implementation, you'd need to handle YouTube's API or scraping
    m_downloadInfo["url"] = url;
    m_downloadInfo["title"] = "YouTube Video"; // Placeholder
    m_downloadInfo["duration"] = 0; // Placeholder
    m_downloadInfo["size"] = 0; // Placeholder
    m_downloadInfo["format"] = "mp4"; // Placeholder
    
    return m_downloadInfo;
}

bool YouTubePlugin::startDownload(const QString &url, const QString &outputPath)
{
    if (!canHandleUrl(url)) {
        emit pluginError("Invalid YouTube URL");
        return false;
    }
    
    m_currentUrl = url;
    m_outputPath = outputPath;
    
    // This is a placeholder implementation
    // Real YouTube downloading requires complex handling of their API
    // and potentially using external tools like youtube-dl or yt-dlp
    
    // For demonstration, we'll simulate a download
    emit downloadProgress(url, 50, 100);
    emit downloadCompleted(url, true, QString());
    
    return true;
}

void YouTubePlugin::onDownloadInfoReceived()
{
    // Handle download info response
    if (!m_currentReply) return;
    
    QByteArray data = m_currentReply->readAll();
    QString html = QString::fromUtf8(data);
    
    m_downloadInfo["title"] = extractVideoTitle(html);
    // In a real implementation, extract more info
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void YouTubePlugin::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(m_currentUrl, bytesReceived, bytesTotal);
}

void YouTubePlugin::onDownloadFinished()
{
    if (!m_currentReply) return;
    
    bool success = (m_currentReply->error() == QNetworkReply::NoError);
    QString errorMessage = success ? QString() : m_currentReply->errorString();
    
    emit downloadCompleted(m_currentUrl, success, errorMessage);
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

bool YouTubePlugin::parseYouTubeUrl(const QString &url, QString &videoId)
{
    QUrl qurl(url);
    if (qurl.host().contains("youtube.com") || qurl.host().contains("youtu.be")) {
        if (qurl.host().contains("youtu.be")) {
            videoId = qurl.path().mid(1); // Remove leading /
        } else {
            QUrlQuery query(qurl);
            videoId = query.queryItemValue("v");
        }
        return !videoId.isEmpty();
    }
    return false;
}

QString YouTubePlugin::extractVideoTitle(const QString &html)
{
    // Simple regex to extract title - this is fragile and not recommended for production
    QRegularExpression regex("<title>(.*?)</title>", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = regex.match(html);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    return "Unknown Title";
}

QString YouTubePlugin::extractDownloadUrl(const QString &html)
{
    // This would require complex parsing of YouTube's player config
    // For this demo, return empty
    return QString();
}