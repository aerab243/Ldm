#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class AutoUpdater : public QObject
{
    Q_OBJECT

public:
    explicit AutoUpdater(QObject *parent = nullptr);
    ~AutoUpdater();

    // Update operations
    void checkForUpdates(const QUrl &updateUrl);
    void downloadUpdate(const QString &downloadUrl, const QString &savePath);
    void installUpdate(const QString &installerPath);

    // Configuration
    void setCurrentVersion(const QString &version);
    QString currentVersion() const;
    void setUpdateCheckInterval(int days);

signals:
    void updateAvailable(const QString &newVersion, const QString &changelog);
    void noUpdateAvailable();
    void updateCheckFailed(const QString &error);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(const QString &filePath);
    void downloadFailed(const QString &error);
    void installationStarted();
    void installationFinished(bool success, const QString &error);

private slots:
    void onUpdateCheckFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onDownloadError(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QString m_currentVersion;
    int m_updateCheckInterval;

    bool parseUpdateInfo(const QJsonDocument &doc, QString &newVersion, QString &changelog);
    QString getPlatformString() const;
};

#endif // AUTOUPDATER_H