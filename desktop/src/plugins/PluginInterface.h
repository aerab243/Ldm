#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class PluginInterface : public QObject
{
    Q_OBJECT

public:
    explicit PluginInterface(QObject *parent = nullptr);
    virtual ~PluginInterface();

    // Plugin information
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;

    // Plugin lifecycle
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;

    // Plugin capabilities
    virtual QStringList supportedProtocols() const = 0;
    virtual bool canHandleUrl(const QString &url) const = 0;

    // Download operations
    virtual QVariantMap getDownloadInfo(const QString &url) = 0;
    virtual bool startDownload(const QString &url, const QString &outputPath) = 0;

signals:
    void downloadProgress(const QString &url, qint64 bytesReceived, qint64 bytesTotal);
    void downloadCompleted(const QString &url, bool success, const QString &errorMessage);
    void pluginError(const QString &error);

private:
    // Disable copy
    PluginInterface(const PluginInterface &) = delete;
    PluginInterface &operator=(const PluginInterface &) = delete;
};

Q_DECLARE_INTERFACE(PluginInterface, "com.ldm.PluginInterface/1.0")

#endif // PLUGININTERFACE_H