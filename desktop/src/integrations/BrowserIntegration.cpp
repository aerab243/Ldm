#include "BrowserIntegration.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>

BrowserIntegration::BrowserIntegration(QObject *parent)
    : QObject(parent)
{
}

BrowserIntegration::~BrowserIntegration()
{
}

QStringList BrowserIntegration::detectInstalledBrowsers()
{
    QStringList browsers;
    
    // Check for Chrome
    if (!QStandardPaths::findExecutable("google-chrome").isEmpty() ||
        !QStandardPaths::findExecutable("chromium").isEmpty()) {
        browsers << "Chrome";
    }
    
    // Check for Firefox
    if (!QStandardPaths::findExecutable("firefox").isEmpty()) {
        browsers << "Firefox";
    }
    
    // Check for Edge
    if (!QStandardPaths::findExecutable("microsoft-edge").isEmpty()) {
        browsers << "Edge";
    }
    
    // Check for Safari (macOS only)
#ifdef Q_OS_MAC
    if (!QStandardPaths::findExecutable("Safari").isEmpty()) {
        browsers << "Safari";
    }
#endif
    
    return browsers;
}

BrowserIntegration::BrowserType BrowserIntegration::getBrowserType(const QString &browserName) const
{
    if (browserName.contains("chrome", Qt::CaseInsensitive)) {
        return Chrome;
    } else if (browserName.contains("firefox", Qt::CaseInsensitive)) {
        return Firefox;
    } else if (browserName.contains("edge", Qt::CaseInsensitive)) {
        return Edge;
    } else if (browserName.contains("safari", Qt::CaseInsensitive)) {
        return Safari;
    }
    return Unknown;
}

QString BrowserIntegration::getBrowserExecutable(BrowserType type) const
{
    switch (type) {
    case Chrome:
        return QStandardPaths::findExecutable("google-chrome").isEmpty() ?
               QStandardPaths::findExecutable("chromium") :
               QStandardPaths::findExecutable("google-chrome");
    case Firefox:
        return QStandardPaths::findExecutable("firefox");
    case Edge:
        return QStandardPaths::findExecutable("microsoft-edge");
    case Safari:
#ifdef Q_OS_MAC
        return "/Applications/Safari.app/Contents/MacOS/Safari";
#else
        return QString();
#endif
    default:
        return QString();
    }
}

bool BrowserIntegration::installExtension(BrowserType type, const QString &extensionPath)
{
    // This is a simplified implementation
    // In reality, extension installation varies greatly between browsers
    Q_UNUSED(type)
    Q_UNUSED(extensionPath)
    
    qWarning() << "Extension installation not implemented for this browser type";
    return false;
}

bool BrowserIntegration::uninstallExtension(BrowserType type, const QString &extensionId)
{
    Q_UNUSED(type)
    Q_UNUSED(extensionId)
    
    qWarning() << "Extension uninstallation not implemented for this browser type";
    return false;
}

bool BrowserIntegration::isExtensionInstalled(BrowserType type, const QString &extensionId)
{
    Q_UNUSED(type)
    Q_UNUSED(extensionId)
    
    // Placeholder implementation
    return false;
}

bool BrowserIntegration::setupNativeMessaging(BrowserType type)
{
    return createNativeMessagingManifest(type);
}

bool BrowserIntegration::registerNativeHost(BrowserType type)
{
    QString manifestPath = getNativeMessagingManifestPath(type);
    if (manifestPath.isEmpty()) {
        return false;
    }
    
    // On Linux, manifests are placed in specific directories
    // This is a simplified implementation
    qInfo() << "Native host registered at:" << manifestPath;
    return true;
}

bool BrowserIntegration::unregisterNativeHost(BrowserType type)
{
    QString manifestPath = getNativeMessagingManifestPath(type);
    if (manifestPath.isEmpty()) {
        return false;
    }
    
    return QFile::remove(manifestPath);
}

bool BrowserIntegration::sendMessageToBrowser(BrowserType type, const QString &message)
{
    Q_UNUSED(type)
    Q_UNUSED(message)
    
    // This would require native messaging setup
    qWarning() << "Sending messages to browser not implemented";
    return false;
}

void BrowserIntegration::handleBrowserMessage(const QString &message)
{
    emit messageReceived(message);
}

QString BrowserIntegration::getChromeExtensionDir() const
{
#ifdef Q_OS_WIN
    return QDir::homePath() + "/AppData/Local/Google/Chrome/User Data/Default/Extensions";
#elif defined(Q_OS_MAC)
    return QDir::homePath() + "/Library/Application Support/Google/Chrome/Default/Extensions";
#else
    return QDir::homePath() + "/.config/google-chrome/Default/Extensions";
#endif
}

QString BrowserIntegration::getFirefoxExtensionDir() const
{
#ifdef Q_OS_WIN
    return QDir::homePath() + "/AppData/Roaming/Mozilla/Firefox/Profiles";
#elif defined(Q_OS_MAC)
    return QDir::homePath() + "/Library/Application Support/Firefox/Profiles";
#else
    return QDir::homePath() + "/.mozilla/firefox";
#endif
}

QString BrowserIntegration::getEdgeExtensionDir() const
{
#ifdef Q_OS_WIN
    return QDir::homePath() + "/AppData/Local/Microsoft/Edge/User Data/Default/Extensions";
#else
    return QString(); // Edge is primarily Windows
#endif
}

QString BrowserIntegration::getSafariExtensionDir() const
{
#ifdef Q_OS_MAC
    return QDir::homePath() + "/Library/Safari/Extensions";
#else
    return QString();
#endif
}

QString BrowserIntegration::getNativeMessagingManifestPath(BrowserType type) const
{
    QString baseDir;
    
    switch (type) {
    case Chrome:
        baseDir = QDir::homePath() + "/.config/google-chrome/NativeMessagingHosts";
        break;
    case Firefox:
        baseDir = QDir::homePath() + "/.mozilla/native-messaging-hosts";
        break;
    case Edge:
        baseDir = QDir::homePath() + "/.config/microsoft-edge/NativeMessagingHosts";
        break;
    default:
        return QString();
    }
    
    QDir().mkpath(baseDir);
    return baseDir + "/com.ldm.native.json";
}

bool BrowserIntegration::createNativeMessagingManifest(BrowserType type)
{
    QString manifestPath = getNativeMessagingManifestPath(type);
    if (manifestPath.isEmpty()) {
        return false;
    }
    
    QJsonObject manifest;
    manifest["name"] = "com.ldm.native";
    manifest["description"] = "LDM Native Messaging Host";
    manifest["path"] = QCoreApplication::applicationDirPath() + "/native-messaging/host/NativeHost";
    manifest["type"] = "stdio";
    
    QJsonObject allowedOrigins;
    allowedOrigins["chrome-extension://*"] = true; // Simplified
    
    manifest["allowed_extensions"] = QJsonArray::fromStringList(QStringList() << "ldm_extension_id");
    
    QJsonDocument doc(manifest);
    
    QFile file(manifestPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create native messaging manifest:" << manifestPath;
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    return true;
}