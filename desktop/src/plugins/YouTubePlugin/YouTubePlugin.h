#ifndef YOUTUBEPLUGIN_H
#define YOUTUBEPLUGIN_H

#include "../PluginInterface.h"
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class YouTubePlugin : public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:
    explicit YouTubePlugin(QObject *parent = nullptr);
    ~YouTubePlugin();

    // PluginInterface implementation
    QString name() const override { return "YouTube Plugin"; }
    QString version() const override { return "1.0.0"; }
    QString description() const override { return "Download videos from YouTube"; }

    bool initialize() override;
    bool shutdown() override;

    QStringList supportedProtocols() const override { return QStringList() << "https"; }
    bool canHandleUrl(const QString &url) const override;

    QVariantMap getDownloadInfo(const QString &url) override;
    bool startDownload(const QString &url, const QString &outputPath) override;

private slots:
    void onDownloadInfoReceived();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QString m_currentUrl;
    QString m_outputPath;
    QVariantMap m_downloadInfo;

    bool parseYouTubeUrl(const QString &url, QString &videoId);
    QString extractVideoTitle(const QString &html);
    QString extractDownloadUrl(const QString &html);
};

#endif // YOUTUBEPLUGIN_H