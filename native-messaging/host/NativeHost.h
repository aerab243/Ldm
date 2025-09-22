#ifndef NATIVEHOST_H
#define NATIVEHOST_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include "MessageParser.h"

class NativeHost : public QObject
{
    Q_OBJECT

public:
    explicit NativeHost(QObject *parent = nullptr);
    ~NativeHost();

    bool start();
    void stop();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onSocketDisconnected();
    void onMessageReceived(const QJsonObject &message);
    void onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadCompleted(int downloadId);
    void onDownloadFailed(int downloadId, const QString &error);

private:
    QTcpServer *m_server;
    QTcpSocket *m_socket;
    MessageParser *m_messageParser;
    QTimer *m_heartbeatTimer;

    void sendMessage(const QJsonObject &message);
    void handleStartDownload(const QJsonObject &data);
    void handleCancelDownload(const QJsonObject &data);
    void handleGetStatus(const QJsonObject &data);
    void sendHeartbeat();

    // Integration with main LDM application
    void notifyMainApplication(const QString &action, const QVariantMap &data);
};

#endif // NATIVEHOST_H