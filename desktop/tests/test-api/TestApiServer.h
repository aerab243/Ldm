#ifndef TESTAPISERVER_H
#define TESTAPISERVER_H

#include <QObject>
#include <QtTest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../../src/core/Database.h"
#include "../../src/core/DownloadEngine.h"
#include "../../src/api/ApiServer.h"

class TestApiServer : public QObject
{
    Q_OBJECT

public:
    TestApiServer();

private:
    QNetworkAccessManager *manager;
    QString baseUrl;
    Database *database;
    DownloadEngine *downloadEngine;
    ApiServer *apiServer;

    // Helper methods
    QJsonDocument getJsonResponse(QNetworkReply *reply);
    void verifyDownloadSchema(const QJsonObject &download);
    void verifyCategorySchema(const QJsonObject &category);
    void verifyHistorySchema(const QJsonObject &history);

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Downloads endpoints
    void testGetDownloads();
    void testPostDownloads();
    void testGetDownloadById();
    void testPutDownloadById();
    void testDeleteDownloadById();
    void testPostDownloadPause();
    void testPostDownloadResume();

    // Categories endpoints
    void testGetCategories();
    void testPostCategories();
    void testGetCategoryById();
    void testPutCategoryById();
    void testDeleteCategoryById();

    // History endpoints
    void testGetHistory();

    // Statistics endpoints
    void testGetStatistics();

    // Settings endpoints
    void testGetSettings();
    void testPutSettings();
};

#endif // TESTAPISERVER_H