#include "ApiServer.h"
#include <QJsonParseError>
#include <QUrlQuery>

ApiServer::ApiServer(Database *database, DownloadEngine *downloadEngine, QObject *parent)
    : QObject(parent)
    , m_tcpServer(new QTcpServer(this))
    , m_httpServer(new QHttpServer(this))
    , m_database(database)
    , m_downloadEngine(downloadEngine)
    , m_cache(new MetadataCache(this))
{
}

ApiServer::~ApiServer()
{
    stop();
}

bool ApiServer::start(quint16 port)
{
    // Downloads routes
    m_httpServer->route("/api/v1/downloads", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &request) {
        return handleGetDownloads(request);
    });
    m_httpServer->route("/api/v1/downloads", QHttpServerRequest::Method::Post, [this](const QHttpServerRequest &request) {
        return handlePostDownloads(request);
    });
    m_httpServer->route("/api/v1/downloads/<arg>", QHttpServerRequest::Method::Get, [this](int id, const QHttpServerRequest &request) {
        return handleGetDownloadById(request, id);
    });
    m_httpServer->route("/api/v1/downloads/<arg>", QHttpServerRequest::Method::Put, [this](int id, const QHttpServerRequest &request) {
        return handlePutDownloadById(request, id);
    });
    m_httpServer->route("/api/v1/downloads/<arg>", QHttpServerRequest::Method::Delete, [this](int id, const QHttpServerRequest &request) {
        return handleDeleteDownloadById(request, id);
    });
    m_httpServer->route("/api/v1/downloads/<arg>/pause", QHttpServerRequest::Method::Post, [this](int id, const QHttpServerRequest &request) {
        return handlePostDownloadPause(request, id);
    });
    m_httpServer->route("/api/v1/downloads/<arg>/resume", QHttpServerRequest::Method::Post, [this](int id, const QHttpServerRequest &request) {
        return handlePostDownloadResume(request, id);
    });

    // Categories routes
    m_httpServer->route("/api/v1/categories", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &request) {
        return handleGetCategories(request);
    });
    m_httpServer->route("/api/v1/categories", QHttpServerRequest::Method::Post, [this](const QHttpServerRequest &request) {
        return handlePostCategories(request);
    });
    m_httpServer->route("/api/v1/categories/<arg>", QHttpServerRequest::Method::Get, [this](int id, const QHttpServerRequest &request) {
        return handleGetCategoryById(request, id);
    });
    m_httpServer->route("/api/v1/categories/<arg>", QHttpServerRequest::Method::Put, [this](int id, const QHttpServerRequest &request) {
        return handlePutCategoryById(request, id);
    });
    m_httpServer->route("/api/v1/categories/<arg>", QHttpServerRequest::Method::Delete, [this](int id, const QHttpServerRequest &request) {
        return handleDeleteCategoryById(request, id);
    });

    // History routes
    m_httpServer->route("/api/v1/history", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &request) {
        return handleGetHistory(request);
    });

    // Statistics routes
    m_httpServer->route("/api/v1/statistics", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &request) {
        return handleGetStatistics(request);
    });

    // Settings routes
    m_httpServer->route("/api/v1/settings", QHttpServerRequest::Method::Get, [this](const QHttpServerRequest &request) {
        return handleGetSettings(request);
    });
    m_httpServer->route("/api/v1/settings", QHttpServerRequest::Method::Put, [this](const QHttpServerRequest &request) {
        return handlePutSettings(request);
    });

    return m_httpServer->bind(m_tcpServer) && m_tcpServer->listen(QHostAddress::Any, port);
}

void ApiServer::stop()
{
    if (m_tcpServer) {
        m_tcpServer->close();
    }
}

QHttpServerResponse ApiServer::handleGetDownloads(const QHttpServerRequest &request)
{
    QUrlQuery query(request.url());
    QString status = query.queryItemValue("status");
    QVariantList downloads = m_database->getDownloads(status);
    
    QJsonArray jsonArray;
    for (const QVariant &variant : downloads) {
        jsonArray.append(downloadToJson(variant.toMap()));
    }
    
    QJsonObject response;
    response["downloads"] = jsonArray;
    response["total"] = downloads.size();
    
    return createJsonResponse(QJsonDocument(response));
}

QHttpServerResponse ApiServer::handlePostDownloads(const QHttpServerRequest &request)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return createErrorResponse("Invalid JSON", 400);
    }
    
    QVariantMap downloadData = jsonToDownload(doc.object());
    int id = m_database->insertDownload(downloadData);
    if (id == -1) {
        return createErrorResponse("Failed to create download", 500);
    }

    // Get the created download
    QVariantMap created = m_database->getDownload(id);
    
    return createJsonResponse(QJsonDocument(downloadToJson(created)), 201);
}

QHttpServerResponse ApiServer::handleGetDownloadById(const QHttpServerRequest &request, int id)
{
    QVariantMap download = m_database->getDownload(id);
    if (download.isEmpty()) {
        return createErrorResponse("Download not found", 404);
    }
    
    return createJsonResponse(QJsonDocument(downloadToJson(download)));
}

QHttpServerResponse ApiServer::handlePutDownloadById(const QHttpServerRequest &request, int id)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return createErrorResponse("Invalid JSON", 400);
    }
    
    QJsonObject json = doc.object();
    QVariantMap updateData;
    if (json.contains("status")) {
        updateData["status"] = json["status"].toString();
    }
    if (json.contains("priority")) {
        updateData["priority"] = json["priority"].toInt();
    }
    if (json.contains("category_id")) {
        updateData["category_id"] = json["category_id"].toInt();
    }
    
    if (!m_database->updateDownload(id, updateData)) {
        return createErrorResponse("Failed to update download", 500);
    }
    
    QVariantMap updated = m_database->getDownload(id);
    return createJsonResponse(QJsonDocument(downloadToJson(updated)));
}

QHttpServerResponse ApiServer::handleDeleteDownloadById(const QHttpServerRequest &request, int id)
{
    if (!m_database->deleteDownload(id)) {
        return createErrorResponse("Failed to delete download", 500);
    }
    
    return QHttpServerResponse(QHttpServerResponse::StatusCode::NoContent);
}

QHttpServerResponse ApiServer::handlePostDownloadPause(const QHttpServerRequest &request, int id)
{
    m_downloadEngine->pauseDownload(id);
    
    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiServer::handlePostDownloadResume(const QHttpServerRequest &request, int id)
{
    m_downloadEngine->resumeDownload(id);
    
    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiServer::handleGetCategories(const QHttpServerRequest &request)
{
    QVariantList categories = m_database->getCategories();
    
    QJsonArray jsonArray;
    for (const QVariant &variant : categories) {
        jsonArray.append(categoryToJson(variant.toMap()));
    }
    
    return createJsonResponse(QJsonDocument(jsonArray));
}

QHttpServerResponse ApiServer::handlePostCategories(const QHttpServerRequest &request)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return createErrorResponse("Invalid JSON", 400);
    }
    
    QVariantMap categoryData = jsonToCategory(doc.object());
    if (!m_database->insertCategory(categoryData)) {
        return createErrorResponse("Failed to create category", 500);
    }
    
    // Get the created category
    int id = categoryData["id"].toInt();
    QVariantMap created = m_database->getCategory(id);
    
    return createJsonResponse(QJsonDocument(categoryToJson(created)), 201);
}

QHttpServerResponse ApiServer::handleGetCategoryById(const QHttpServerRequest &request, int id)
{
    QVariantMap category = m_database->getCategory(id);
    if (category.isEmpty()) {
        return createErrorResponse("Category not found", 404);
    }
    
    return createJsonResponse(QJsonDocument(categoryToJson(category)));
}

QHttpServerResponse ApiServer::handlePutCategoryById(const QHttpServerRequest &request, int id)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return createErrorResponse("Invalid JSON", 400);
    }
    
    QJsonObject json = doc.object();
    QVariantMap updateData;
    if (json.contains("name")) {
        updateData["name"] = json["name"].toString();
    }
    if (json.contains("description")) {
        updateData["description"] = json["description"].toString();
    }
    if (json.contains("default_path")) {
        updateData["default_path"] = json["default_path"].toString();
    }
    
    if (!m_database->updateCategory(id, updateData)) {
        return createErrorResponse("Failed to update category", 500);
    }
    
    QVariantMap updated = m_database->getCategory(id);
    return createJsonResponse(QJsonDocument(categoryToJson(updated)));
}

QHttpServerResponse ApiServer::handleDeleteCategoryById(const QHttpServerRequest &request, int id)
{
    if (!m_database->deleteCategory(id)) {
        return createErrorResponse("Failed to delete category", 500);
    }
    
    return QHttpServerResponse(QHttpServerResponse::StatusCode::NoContent);
}

QHttpServerResponse ApiServer::handleGetHistory(const QHttpServerRequest &request)
{
    QUrlQuery query(request.url());
    int limit = query.queryItemValue("limit").isEmpty() ? 100 : query.queryItemValue("limit").toInt();
    int offset = query.queryItemValue("offset").isEmpty() ? 0 : query.queryItemValue("offset").toInt();
    
    QVariantList history = m_database->getDownloadHistory(limit, offset);
    
    QJsonArray jsonArray;
    for (const QVariant &variant : history) {
        jsonArray.append(historyToJson(variant.toMap()));
    }
    
    QJsonObject response;
    response["history"] = jsonArray;
    response["total"] = history.size();
    
    return createJsonResponse(QJsonDocument(response));
}

QHttpServerResponse ApiServer::handleGetStatistics(const QHttpServerRequest &request)
{
    QString cacheKey = "statistics";
    
    // Check cache first
    if (m_cache->contains(cacheKey)) {
        QVariantMap cachedStats = m_cache->retrieve(cacheKey);
        QJsonObject stats;
        for (auto it = cachedStats.begin(); it != cachedStats.end(); ++it) {
            stats[it.key()] = QJsonValue::fromVariant(it.value());
        }
        return createJsonResponse(QJsonDocument(stats));
    }
    
    // Compute statistics
    QVariantList downloads = m_database->getDownloads();
    QVariantList history = m_database->getDownloadHistory(1000, 0);
    
    int totalDownloads = downloads.size();
    qint64 totalSize = 0;
    int activeDownloads = 0;
    int completedToday = 0;
    qint64 totalSpeed = 0;
    int speedCount = 0;
    
    QDateTime today = QDateTime::currentDateTime().date().startOfDay();
    
    for (const QVariant &variant : downloads) {
        QVariantMap download = variant.toMap();
        totalSize += download["total_size"].toLongLong();
        if (download["status"].toString() == "downloading") {
            activeDownloads++;
        }
        if (download.contains("speed") && download["speed"].toInt() > 0) {
            totalSpeed += download["speed"].toInt();
            speedCount++;
        }
    }
    
    for (const QVariant &variant : history) {
        QVariantMap entry = variant.toMap();
        if (entry["completed_at"].toDateTime() >= today) {
            completedToday++;
        }
    }
    
    QJsonObject stats;
    stats["total_downloads"] = totalDownloads;
    stats["total_size"] = totalSize;
    stats["active_downloads"] = activeDownloads;
    stats["completed_today"] = completedToday;
    stats["average_speed"] = speedCount > 0 ? totalSpeed / speedCount : 0;
    
    // Cache the result for 5 minutes
    QVariantMap statsMap;
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        statsMap[it.key()] = it.value().toVariant();
    }
    m_cache->store(cacheKey, statsMap);
    
    return createJsonResponse(QJsonDocument(stats));
}

QHttpServerResponse ApiServer::handleGetSettings(const QHttpServerRequest &request)
{
    QUrlQuery query(request.url());
    QString category = query.queryItemValue("category");
    
    QVariantList settings = m_database->getSettings(category);
    
    QJsonObject jsonSettings;
    for (const QVariant &variant : settings) {
        QVariantMap setting = variant.toMap();
        jsonSettings[setting["key"].toString()] = QJsonValue::fromVariant(setting["value"]);
    }
    
    return createJsonResponse(QJsonDocument(jsonSettings));
}

QHttpServerResponse ApiServer::handlePutSettings(const QHttpServerRequest &request)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(request.body(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return createErrorResponse("Invalid JSON", 400);
    }
    
    QJsonObject json = doc.object();
    for (auto it = json.begin(); it != json.end(); ++it) {
        m_database->setSetting(it.key(), it.value().toVariant());
    }
    
    return QHttpServerResponse(QHttpServerResponse::StatusCode::Ok);
}

QJsonObject ApiServer::downloadToJson(const QVariantMap &download)
{
    QJsonObject obj;
    obj["id"] = download["id"].toInt();
    obj["url"] = download["url"].toString();
    obj["filename"] = download["filename"].toString();
    obj["filepath"] = download["filepath"].toString();
    obj["status"] = download["status"].toString();
    obj["progress"] = download["progress"].toDouble();
    obj["total_size"] = download["total_size"].toLongLong();
    obj["downloaded_size"] = download["downloaded_size"].toLongLong();
    obj["speed"] = download["speed"].toInt();
    obj["eta"] = download["eta"].toInt();
    obj["created_at"] = download["created_at"].toString();
    obj["category_id"] = download["category_id"].toInt();
    obj["priority"] = download["priority"].toInt();
    return obj;
}

QJsonObject ApiServer::categoryToJson(const QVariantMap &category)
{
    QJsonObject obj;
    obj["id"] = category["id"].toInt();
    obj["name"] = category["name"].toString();
    obj["description"] = category["description"].toString();
    obj["default_path"] = category["default_path"].toString();
    obj["color"] = category["color"].toString();
    obj["icon"] = category["icon"].toString();
    return obj;
}

QJsonObject ApiServer::historyToJson(const QVariantMap &history)
{
    QJsonObject obj;
    obj["id"] = history["id"].toInt();
    obj["url"] = history["url"].toString();
    obj["filename"] = history["filename"].toString();
    obj["size"] = history["size"].toLongLong();
    obj["completed_at"] = history["completed_at"].toString();
    obj["duration"] = history["duration"].toInt();
    obj["average_speed"] = history["average_speed"].toInt();
    obj["success"] = history["success"].toBool();
    return obj;
}

QVariantMap ApiServer::jsonToDownload(const QJsonObject &json)
{
    QVariantMap map;
    if (json.contains("url")) map["url"] = json["url"].toString();
    if (json.contains("filename")) map["filename"] = json["filename"].toString();
    if (json.contains("category_id")) map["category_id"] = json["category_id"].toInt();
    if (json.contains("priority")) map["priority"] = json["priority"].toInt();
    if (json.contains("segments")) map["segments"] = json["segments"].toInt();
    map["status"] = "queued";
    map["progress"] = 0.0;
    map["downloaded_size"] = 0;
    return map;
}

QVariantMap ApiServer::jsonToCategory(const QJsonObject &json)
{
    QVariantMap map;
    if (json.contains("name")) map["name"] = json["name"].toString();
    if (json.contains("description")) map["description"] = json["description"].toString();
    if (json.contains("default_path")) map["default_path"] = json["default_path"].toString();
    return map;
}

QHttpServerResponse ApiServer::createJsonResponse(const QJsonDocument &doc, int status)
{
    return QHttpServerResponse("application/json", doc.toJson(), QHttpServerResponse::StatusCode(status));
}

QHttpServerResponse ApiServer::createErrorResponse(const QString &message, int status)
{
    QJsonObject error;
    error["error"] = message;
    QJsonObject errorObj;
    errorObj["error"] = message;
    return QHttpServerResponse("application/json", QJsonDocument(errorObj).toJson(), QHttpServerResponse::StatusCode(status));
}