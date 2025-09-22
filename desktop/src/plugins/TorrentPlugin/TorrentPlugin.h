#ifndef TORRENTPLUGIN_H
#define TORRENTPLUGIN_H

#include "../PluginInterface.h"
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class TorrentPlugin : public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:
    explicit TorrentPlugin(QObject *parent = nullptr);
    ~TorrentPlugin();

    // PluginInterface implementation
    QString name() const override { return "Torrent Plugin"; }
    QString version() const override { return "1.0.0"; }
    QString description() const override { return "Download torrents"; }

    bool initialize() override;
    bool shutdown() override;

    QStringList supportedProtocols() const override { return QStringList() << "magnet" << "torrent"; }
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

    bool parseTorrentUrl(const QString &url);
    QString extractTorrentInfo(const QByteArray &data);
};

#endif // TORRENTPLUGIN_H