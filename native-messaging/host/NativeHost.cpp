#include "NativeHost.h"
#include <QCoreApplication>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

NativeHost::NativeHost(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_socket(nullptr)
    , m_messageParser(new MessageParser(this))
    , m_heartbeatTimer(new QTimer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &NativeHost::onNewConnection);
    connect(m_messageParser, &MessageParser::messageReceived, this, &NativeHost::onMessageReceived);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &NativeHost::sendHeartbeat);
}

NativeHost::~NativeHost()
{
    stop();
}

bool NativeHost::start()
{
    if (!m_server->listen(QHostAddress::LocalHost, 0)) {
        qCritical() << "Failed to start native messaging server:" << m_server->errorString();
        return false;
    }

    qDebug() << "Native messaging server started on port" << m_server->serverPort();

    // Print the port to stdout for the browser extension to read
    QTextStream(stdout) << m_server->serverPort() << Qt::endl;

    m_heartbeatTimer->start(30000); // Send heartbeat every 30 seconds

    return true;
}

void NativeHost::stop()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
    }
    m_server->close();
    m_heartbeatTimer->stop();
}

void NativeHost::onNewConnection()
{
    if (m_socket) {
        // Only allow one connection at a time
        QTcpSocket *newSocket = m_server->nextPendingConnection();
        newSocket->disconnectFromHost();
        newSocket->deleteLater();
        return;
    }

    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &NativeHost::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NativeHost::onSocketDisconnected);

    qDebug() << "Browser extension connected";
}

void NativeHost::onReadyRead()
{
    if (!m_socket) return;

    QByteArray data = m_socket->readAll();
    m_messageParser->parseMessage(data);
}

void NativeHost::onSocketDisconnected()
{
    qDebug() << "Browser extension disconnected";
    if (m_socket) {
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void NativeHost::onMessageReceived(const QJsonObject &message)
{
    QString type = message.value("type").toString();

    if (type == "START_DOWNLOAD") {
        handleStartDownload(message.value("data").toObject());
    } else if (type == "CANCEL_DOWNLOAD") {
        handleCancelDownload(message.value("data").toObject());
    } else if (type == "GET_STATUS") {
        handleGetStatus(message.value("data").toObject());
    } else {
        qWarning() << "Unknown message type:" << type;
    }
}

void NativeHost::handleStartDownload(const QJsonObject &data)
{
    QString url = data.value("url").toString();
    QString filename = data.value("filename").toString();
    QString referrer = data.value("referrer").toString();
    QString userAgent = data.value("userAgent").toString();

    // Notify main LDM application
    QVariantMap downloadData;
    downloadData["url"] = url;
    downloadData["filename"] = filename;
    downloadData["referrer"] = referrer;
    downloadData["userAgent"] = userAgent;

    notifyMainApplication("start_download", downloadData);

    // Send response
    QJsonObject response;
    response["type"] = "DOWNLOAD_STARTED";
    response["data"] = QJsonObject{
        {"id", 1}, // Would be actual download ID
        {"filename", filename}
    };
    sendMessage(response);
}

void NativeHost::handleCancelDownload(const QJsonObject &data)
{
    int downloadId = data.value("downloadId").toInt();

    notifyMainApplication("cancel_download", {{"downloadId", downloadId}});

    QJsonObject response;
    response["type"] = "DOWNLOAD_CANCELLED";
    response["data"] = QJsonObject{{"downloadId", downloadId}};
    sendMessage(response);
}

void NativeHost::handleGetStatus(const QJsonObject &data)
{
    int downloadId = data.value("downloadId").toInt();

    // Query main application for status
    // For now, send mock response
    QJsonObject response;
    response["type"] = "DOWNLOAD_STATUS";
    response["data"] = QJsonObject{
        {"downloadId", downloadId},
        {"status", "downloading"},
        {"progress", 50}
    };
    sendMessage(response);
}

void NativeHost::sendMessage(const QJsonObject &message)
{
    if (!m_socket) return;

    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    // Prepend message length (native messaging protocol)
    QByteArray lengthBytes;
    lengthBytes.append((data.size() >> 0) & 0xFF);
    lengthBytes.append((data.size() >> 8) & 0xFF);
    lengthBytes.append((data.size() >> 16) & 0xFF);
    lengthBytes.append((data.size() >> 24) & 0xFF);

    m_socket->write(lengthBytes + data);
}

void NativeHost::sendHeartbeat()
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        QJsonObject heartbeat;
        heartbeat["type"] = "HEARTBEAT";
        sendMessage(heartbeat);
    }
}

void NativeHost::notifyMainApplication(const QString &action, const QVariantMap &data)
{
    // This would integrate with the main LDM application
    // For example, via DBus, shared memory, or local socket
    qDebug() << "Notifying main application:" << action << data;
}

void NativeHost::onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal)
{
    QJsonObject message;
    message["type"] = "DOWNLOAD_PROGRESS";
    message["data"] = QJsonObject{
        {"downloadId", downloadId},
        {"bytesReceived", bytesReceived},
        {"bytesTotal", bytesTotal},
        {"progress", bytesTotal > 0 ? (bytesReceived * 100.0 / bytesTotal) : 0}
    };
    sendMessage(message);
}

void NativeHost::onDownloadCompleted(int downloadId)
{
    QJsonObject message;
    message["type"] = "DOWNLOAD_COMPLETED";
    message["data"] = QJsonObject{{"downloadId", downloadId}};
    sendMessage(message);
}

void NativeHost::onDownloadFailed(int downloadId, const QString &error)
{
    QJsonObject message;
    message["type"] = "DOWNLOAD_FAILED";
    message["data"] = QJsonObject{
        {"downloadId", downloadId},
        {"error", error}
    };
    sendMessage(message);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    NativeHost host;
    if (!host.start()) {
        return 1;
    }

    return app.exec();
}