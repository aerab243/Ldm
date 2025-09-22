#ifndef BROWSERINTEGRATION_H
#define BROWSERINTEGRATION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>

class BrowserIntegration : public QObject
{
    Q_OBJECT

public:
    enum BrowserType {
        Chrome,
        Firefox,
        Edge,
        Safari,
        Unknown
    };

    explicit BrowserIntegration(QObject *parent = nullptr);
    ~BrowserIntegration();

    // Browser detection
    QStringList detectInstalledBrowsers();
    BrowserType getBrowserType(const QString &browserName) const;
    QString getBrowserExecutable(BrowserType type) const;

    // Extension management
    bool installExtension(BrowserType type, const QString &extensionPath);
    bool uninstallExtension(BrowserType type, const QString &extensionId);
    bool isExtensionInstalled(BrowserType type, const QString &extensionId);

    // Native messaging
    bool setupNativeMessaging(BrowserType type);
    bool registerNativeHost(BrowserType type);
    bool unregisterNativeHost(BrowserType type);

    // Communication
    bool sendMessageToBrowser(BrowserType type, const QString &message);
    void handleBrowserMessage(const QString &message);

signals:
    void browserDetected(BrowserType type, const QString &version);
    void extensionInstalled(BrowserType type, const QString &extensionId);
    void extensionUninstalled(BrowserType type, const QString &extensionId);
    void messageReceived(const QString &message);

private:
    QString getChromeExtensionDir() const;
    QString getFirefoxExtensionDir() const;
    QString getEdgeExtensionDir() const;
    QString getSafariExtensionDir() const;

    QString getNativeMessagingManifestPath(BrowserType type) const;
    bool createNativeMessagingManifest(BrowserType type);
};

#endif // BROWSERINTEGRATION_H