#include "AutoUpdater.h"
#include <QJsonParseError>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>
#include <QSysInfo>

AutoUpdater::AutoUpdater(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_updateCheckInterval(7) // Default: check weekly
{
}

AutoUpdater::~AutoUpdater()
{
    if (m_currentReply) {
        m_currentReply->abort();
    }
}

void AutoUpdater::checkForUpdates(const QUrl &updateUrl)
{
    if (m_currentReply) {
        m_currentReply->abort();
    }

    QNetworkRequest request(updateUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "LDM/" + m_currentVersion);

    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::finished, this, &AutoUpdater::onUpdateCheckFinished);
}

void AutoUpdater::downloadUpdate(const QString &downloadUrl, const QString &savePath)
{
    if (m_currentReply) {
        m_currentReply->abort();
    }

    QNetworkRequest request(QUrl(downloadUrl));
    m_currentReply = m_networkManager->get(request);

    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &AutoUpdater::onDownloadProgress);
    connect(m_currentReply, &QNetworkReply::finished, this, &AutoUpdater::onDownloadFinished);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &AutoUpdater::onDownloadError);

    // For simplicity, assume savePath is handled in onDownloadFinished
    m_currentReply->setProperty("savePath", savePath);
}

void AutoUpdater::installUpdate(const QString &installerPath)
{
    emit installationStarted();

    QProcess *installerProcess = new QProcess(this);
    connect(installerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, installerProcess](int exitCode, QProcess::ExitStatus exitStatus) {
                bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);
                QString error = success ? QString() : installerProcess->errorString();
                emit installationFinished(success, error);
                installerProcess->deleteLater();
            });

    // Run installer with appropriate arguments based on platform
    QStringList arguments;
#ifdef Q_OS_WIN
    arguments << "/S"; // Silent install for NSIS
#endif
#ifdef Q_OS_MAC
    arguments << "-q"; // Quiet install for macOS
#endif
#ifdef Q_OS_LINUX
    arguments << "--noexec"; // For DEB/RPM, might need different handling
#endif

    installerProcess->start(installerPath, arguments);
}

void AutoUpdater::setCurrentVersion(const QString &version)
{
    m_currentVersion = version;
}

QString AutoUpdater::currentVersion() const
{
    return m_currentVersion;
}

void AutoUpdater::setUpdateCheckInterval(int days)
{
    m_updateCheckInterval = days;
}

void AutoUpdater::onUpdateCheckFinished()
{
    if (!m_currentReply) return;

    if (m_currentReply->error() != QNetworkReply::NoError) {
        emit updateCheckFailed(m_currentReply->errorString());
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    QByteArray data = m_currentReply->readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        emit updateCheckFailed("Invalid JSON response: " + error.errorString());
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    QString newVersion, changelog;
    if (parseUpdateInfo(doc, newVersion, changelog)) {
        if (newVersion > m_currentVersion) {
            emit updateAvailable(newVersion, changelog);
        } else {
            emit noUpdateAvailable();
        }
    } else {
        emit updateCheckFailed("Failed to parse update information");
    }

    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void AutoUpdater::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

void AutoUpdater::onDownloadFinished()
{
    if (!m_currentReply) return;

    QString savePath = m_currentReply->property("savePath").toString();
    if (savePath.isEmpty()) {
        emit downloadFailed("No save path specified");
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    if (m_currentReply->error() != QNetworkReply::NoError) {
        emit downloadFailed(m_currentReply->errorString());
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit downloadFailed("Failed to open file for writing: " + savePath);
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    file.write(m_currentReply->readAll());
    file.close();

    emit downloadFinished(savePath);

    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void AutoUpdater::onDownloadError(QNetworkReply::NetworkError error)
{
    emit downloadFailed(m_currentReply->errorString());
}

bool AutoUpdater::parseUpdateInfo(const QJsonDocument &doc, QString &newVersion, QString &changelog)
{
    if (!doc.isObject()) return false;

    QJsonObject obj = doc.object();
    QString platform = getPlatformString();

    if (!obj.contains(platform)) return false;

    QJsonObject platformInfo = obj[platform].toObject();
    newVersion = platformInfo["version"].toString();
    changelog = platformInfo["changelog"].toString();

    return !newVersion.isEmpty();
}

QString AutoUpdater::getPlatformString() const
{
#ifdef Q_OS_WIN
    return "windows";
#elif defined(Q_OS_MAC)
    return "macos";
#elif defined(Q_OS_LINUX)
    return "linux";
#else
    return "unknown";
#endif
}