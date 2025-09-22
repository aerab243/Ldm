#ifndef APISERVER_H
#define APISERVER_H

#include <QObject>
#include <QHttpServer>
#include <QTcpServer>
#include <QHttpServerResponse>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "core/Database.h"
#include "core/DownloadEngine.h"
#include "utils/MetadataCache.h"

class ApiServer : public QObject
{
    Q_OBJECT

public:
    explicit ApiServer(Database *database, DownloadEngine *downloadEngine, QObject *parent = nullptr);
    ~ApiServer();

    bool start(quint16 port = 8080);
    void stop();

private slots:
    // Downloads endpoints
    QHttpServerResponse handleGetDownloads(const QHttpServerRequest &request);
    QHttpServerResponse handlePostDownloads(const QHttpServerRequest &request);
    QHttpServerResponse handleGetDownloadById(const QHttpServerRequest &request, int id);
    QHttpServerResponse handlePutDownloadById(const QHttpServerRequest &request, int id);
    QHttpServerResponse handleDeleteDownloadById(const QHttpServerRequest &request, int id);
    QHttpServerResponse handlePostDownloadPause(const QHttpServerRequest &request, int id);
    QHttpServerResponse handlePostDownloadResume(const QHttpServerRequest &request, int id);

    // Categories endpoints
    QHttpServerResponse handleGetCategories(const QHttpServerRequest &request);
    QHttpServerResponse handlePostCategories(const QHttpServerRequest &request);
    QHttpServerResponse handleGetCategoryById(const QHttpServerRequest &request, int id);
    QHttpServerResponse handlePutCategoryById(const QHttpServerRequest &request, int id);
    QHttpServerResponse handleDeleteCategoryById(const QHttpServerRequest &request, int id);

    // History endpoints
    QHttpServerResponse handleGetHistory(const QHttpServerRequest &request);

    // Statistics endpoints
    QHttpServerResponse handleGetStatistics(const QHttpServerRequest &request);

    // Settings endpoints
    QHttpServerResponse handleGetSettings(const QHttpServerRequest &request);
    QHttpServerResponse handlePutSettings(const QHttpServerRequest &request);

private:
    QTcpServer *m_tcpServer;
    QHttpServer *m_httpServer;
    Database *m_database;
    DownloadEngine *m_downloadEngine;
    MetadataCache *m_cache;

    QJsonObject downloadToJson(const QVariantMap &download);
    QJsonObject categoryToJson(const QVariantMap &category);
    QJsonObject historyToJson(const QVariantMap &history);
    QVariantMap jsonToDownload(const QJsonObject &json);
    QVariantMap jsonToCategory(const QJsonObject &json);
    QHttpServerResponse createJsonResponse(const QJsonDocument &doc, int status = 200);
    QHttpServerResponse createErrorResponse(const QString &message, int status = 400);
};

#endif // APISERVER_H