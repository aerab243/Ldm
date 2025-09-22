#include "TestApiServer.h"

TestApiServer::TestApiServer() : manager(nullptr), baseUrl("http://localhost:8080/api/v1") {}

void TestApiServer::initTestCase()
{
    manager = new QNetworkAccessManager(this);
    
    // Create database and download engine
    database = new Database(this);
    downloadEngine = new DownloadEngine(this);
    
    // Open database
    QString dbPath = "/tmp/test_ldm.db";
    QVERIFY(database->open(dbPath));
    
    apiServer = new ApiServer(database, downloadEngine, this);
    
    // Start API server
    QVERIFY(apiServer->start(8080));
}

void TestApiServer::cleanupTestCase()
{
    if (apiServer) {
        apiServer->stop();
        delete apiServer;
    }
    delete downloadEngine;
    delete database;
    delete manager;
}

QJsonDocument TestApiServer::getJsonResponse(QNetworkReply *reply)
{
    if (!reply || reply->error() != QNetworkReply::NoError) {
        return QJsonDocument();
    }

    QByteArray data = reply->readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        return QJsonDocument();
    }

    return doc;
}

void TestApiServer::verifyDownloadSchema(const QJsonObject &download)
{
    QVERIFY(download.contains("id"));
    QVERIFY(download["id"].isDouble());
    QVERIFY(download.contains("url"));
    QVERIFY(download["url"].isString());
    QVERIFY(download.contains("status"));
    QVERIFY(download["status"].isString());
    // Add more schema validations as per api.yaml
}

void TestApiServer::verifyCategorySchema(const QJsonObject &category)
{
    QVERIFY(category.contains("id"));
    QVERIFY(category["id"].isDouble());
    QVERIFY(category.contains("name"));
    QVERIFY(category["name"].isString());
}

void TestApiServer::verifyHistorySchema(const QJsonObject &history)
{
    QVERIFY(history.contains("id"));
    QVERIFY(history["id"].isDouble());
    QVERIFY(history.contains("url"));
    QVERIFY(history["url"].isString());
    QVERIFY(history.contains("completed_at"));
    QVERIFY(history["completed_at"].isString());
}

void TestApiServer::testGetDownloads()
{
    QNetworkRequest request(QUrl(baseUrl + "/downloads"));
    QNetworkReply *reply = manager->get(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QJsonDocument doc = getJsonResponse(reply);
    QVERIFY(doc.isObject());
    QJsonObject obj = doc.object();
    QVERIFY(obj.contains("downloads"));
    QVERIFY(obj["downloads"].isArray());
    
    QJsonArray downloads = obj["downloads"].toArray();
    for (const QJsonValue &value : downloads) {
        QVERIFY(value.isObject());
        verifyDownloadSchema(value.toObject());
    }
    
    reply->deleteLater();
}

void TestApiServer::testPostDownloads()
{
    QNetworkRequest request(QUrl(baseUrl + "/downloads"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject download;
    download["url"] = "http://example.com/test.txt";
    download["filename"] = "test.txt";
    QJsonDocument doc(download);
    
    QNetworkReply *reply = manager->post(request, doc.toJson());
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 201);
    
    QJsonDocument responseDoc = getJsonResponse(reply);
    QVERIFY(responseDoc.isObject());
    verifyDownloadSchema(responseDoc.object());
    
    reply->deleteLater();
}

void TestApiServer::testGetDownloadById()
{
    // Assume we have a download with id 1 from previous test
    QNetworkRequest request(QUrl(baseUrl + "/downloads/1"));
    QNetworkReply *reply = manager->get(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QJsonDocument doc = getJsonResponse(reply);
    QVERIFY(doc.isObject());
    verifyDownloadSchema(doc.object());
    
    reply->deleteLater();
}

void TestApiServer::testPutDownloadById()
{
    QNetworkRequest request(QUrl(baseUrl + "/downloads/1"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject update;
    update["status"] = "paused";
    QJsonDocument doc(update);
    
    QNetworkReply *reply = manager->put(request, doc.toJson());
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    
    QJsonDocument responseDoc = getJsonResponse(reply);
    QVERIFY(responseDoc.isObject());
    verifyDownloadSchema(responseDoc.object());
    
    reply->deleteLater();
}

void TestApiServer::testDeleteDownloadById()
{
    QNetworkRequest request(QUrl(baseUrl + "/downloads/1"));
    QNetworkReply *reply = manager->deleteResource(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 204);
    
    reply->deleteLater();
}

void TestApiServer::testPostDownloadPause()
{
    QNetworkRequest request(QUrl(baseUrl + "/downloads/1/pause"));
    QNetworkReply *reply = manager->post(request, QByteArray());
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    
    reply->deleteLater();
}

void TestApiServer::testPostDownloadResume()
{
    QNetworkRequest request(QUrl(baseUrl + "/downloads/1/resume"));
    QNetworkReply *reply = manager->post(request, QByteArray());
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    
    reply->deleteLater();
}

void TestApiServer::testGetCategories()
{
    QNetworkRequest request(QUrl(baseUrl + "/categories"));
    QNetworkReply *reply = manager->get(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QJsonDocument doc = getJsonResponse(reply);
    QVERIFY(doc.isArray());
    
    QJsonArray categories = doc.array();
    for (const QJsonValue &value : categories) {
        QVERIFY(value.isObject());
        verifyCategorySchema(value.toObject());
    }
    
    reply->deleteLater();
}

void TestApiServer::testPostCategories()
{
    QNetworkRequest request(QUrl(baseUrl + "/categories"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject category;
    category["name"] = "Test Category";
    category["description"] = "Test description";
    QJsonDocument doc(category);
    
    QNetworkReply *reply = manager->post(request, doc.toJson());
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 201);
    
    QJsonDocument responseDoc = getJsonResponse(reply);
    QVERIFY(responseDoc.isObject());
    verifyCategorySchema(responseDoc.object());
    
    reply->deleteLater();
}

void TestApiServer::testGetCategoryById()
{
    QNetworkRequest request(QUrl(baseUrl + "/categories/1"));
    QNetworkReply *reply = manager->get(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QJsonDocument doc = getJsonResponse(reply);
    QVERIFY(doc.isObject());
    verifyCategorySchema(doc.object());
    
    reply->deleteLater();
}

void TestApiServer::testPutCategoryById()
{
    QNetworkRequest request(QUrl(baseUrl + "/categories/1"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject update;
    update["name"] = "Updated Category";
    QJsonDocument doc(update);
    
    QNetworkReply *reply = manager->put(request, doc.toJson());
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    
    QJsonDocument responseDoc = getJsonResponse(reply);
    QVERIFY(responseDoc.isObject());
    verifyCategorySchema(responseDoc.object());
    
    reply->deleteLater();
}

void TestApiServer::testDeleteCategoryById()
{
    QNetworkRequest request(QUrl(baseUrl + "/categories/1"));
    QNetworkReply *reply = manager->deleteResource(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 204);
    
    reply->deleteLater();
}

void TestApiServer::testGetHistory()
{
    QNetworkRequest request(QUrl(baseUrl + "/history"));
    QNetworkReply *reply = manager->get(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QJsonDocument doc = getJsonResponse(reply);
    QVERIFY(doc.isObject());
    QJsonObject obj = doc.object();
    QVERIFY(obj.contains("history"));
    QVERIFY(obj["history"].isArray());
    
    QJsonArray history = obj["history"].toArray();
    for (const QJsonValue &value : history) {
        QVERIFY(value.isObject());
        verifyHistorySchema(value.toObject());
    }
    
    reply->deleteLater();
}

void TestApiServer::testGetStatistics()
{
    QNetworkRequest request(QUrl(baseUrl + "/statistics"));
    QNetworkReply *reply = manager->get(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QJsonDocument doc = getJsonResponse(reply);
    QVERIFY(doc.isObject());
    QJsonObject obj = doc.object();
    QVERIFY(obj.contains("total_downloads"));
    QVERIFY(obj["total_downloads"].isDouble());
    // Add more statistics validations
    
    reply->deleteLater();
}

void TestApiServer::testGetSettings()
{
    QNetworkRequest request(QUrl(baseUrl + "/settings"));
    QNetworkReply *reply = manager->get(request);
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QJsonDocument doc = getJsonResponse(reply);
    QVERIFY(doc.isObject());
    
    reply->deleteLater();
}

void TestApiServer::testPutSettings()
{
    QNetworkRequest request(QUrl(baseUrl + "/settings"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject settings;
    settings["theme"] = "dark";
    QJsonDocument doc(settings);
    
    QNetworkReply *reply = manager->put(request, doc.toJson());
    QSignalSpy spy(reply, &QNetworkReply::finished);
    QVERIFY(spy.wait(5000));
    
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    
    reply->deleteLater();
}