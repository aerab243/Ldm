#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>

#include "core/Database.h"
#include "core/DownloadEngine.h"
#include "core/DownloadItem.h"
#include "core/NetworkManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("LDM CLI");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("LDM");

    QCommandLineParser parser;
    parser.setApplicationDescription("LDM Download Manager Command Line Interface");
    parser.addHelpOption();
    parser.addVersionOption();

    // Commands
    QCommandLineOption addOption("add", "Add a new download", "url");
    QCommandLineOption listOption("list", "List downloads");
    QCommandLineOption pauseOption("pause", "Pause download", "id");
    QCommandLineOption resumeOption("resume", "Resume download", "id");
    QCommandLineOption cancelOption("cancel", "Cancel download", "id");
    QCommandLineOption statusOption("status", "Show download status", "id");
    QCommandLineOption historyOption("history", "Show download history");

    parser.addOption(addOption);
    parser.addOption(listOption);
    parser.addOption(pauseOption);
    parser.addOption(resumeOption);
    parser.addOption(cancelOption);
    parser.addOption(statusOption);
    parser.addOption(historyOption);

    parser.process(app);

    // Initialize database
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    QString dbPath = dataDir + "/ldm.db";

    Database database;
    if (!database.open(dbPath)) {
        qCritical() << "Failed to open database";
        return 1;
    }

    if (!database.createTables()) {
        qCritical() << "Failed to create database tables";
        return 1;
    }

    // Initialize download engine
    DownloadEngine downloadEngine;

    // Process commands
    if (parser.isSet(addOption)) {
        QString url = parser.value(addOption);
        if (url.isEmpty()) {
            qCritical() << "URL is required for --add";
            return 1;
        }

        QVariantMap downloadData;
        downloadData["url"] = url;
        downloadData["status"] = "queued";
        downloadData["progress"] = 0.0;
        downloadData["downloaded_size"] = 0;

        int id = database.insertDownload(downloadData);
        if (id != -1) {
            qInfo() << "Download added successfully";
            QVariantMap download = database.getDownload(id);
            DownloadItem *item = new DownloadItem(id, download["url"].toString(), download["filename"].toString());
            item->setFilepath(download["filepath"].toString());
            item->setStatus(download["status"].toString());
            item->setTotalSize(download["total_size"].toLongLong());
            item->setDownloadedSize(download["downloaded_size"].toLongLong());
            item->setCreatedAt(download["created_at"].toDateTime());
            item->setCategoryId(download["category_id"].toInt());
            item->setPriority(download["priority"].toInt());
            downloadEngine.startDownload(item);
        } else {
            qCritical() << "Failed to add download";
            return 1;
        }
    } else if (parser.isSet(listOption)) {
        QVariantList downloads = database.getDownloads();
        qInfo() << "Downloads:";
        for (const QVariant &variant : downloads) {
            QVariantMap download = variant.toMap();
            qInfo() << QString("ID: %1, URL: %2, Status: %3, Progress: %4%")
                       .arg(download["id"].toInt())
                       .arg(download["url"].toString())
                       .arg(download["status"].toString())
                       .arg(download["progress"].toDouble() * 100);
        }
    } else if (parser.isSet(pauseOption)) {
        int id = parser.value(pauseOption).toInt();
        if (id == 0) {
            qCritical() << "Valid ID required for --pause";
            return 1;
        }
        downloadEngine.pauseDownload(id);
        qInfo() << "Download paused";
    } else if (parser.isSet(resumeOption)) {
        int id = parser.value(resumeOption).toInt();
        if (id == 0) {
            qCritical() << "Valid ID required for --resume";
            return 1;
        }
        downloadEngine.resumeDownload(id);
        qInfo() << "Download resumed";
    } else if (parser.isSet(cancelOption)) {
        int id = parser.value(cancelOption).toInt();
        if (id == 0) {
            qCritical() << "Valid ID required for --cancel";
            return 1;
        }
        downloadEngine.cancelDownload(id);
        qInfo() << "Download cancelled";
    } else if (parser.isSet(statusOption)) {
        int id = parser.value(statusOption).toInt();
        if (id == 0) {
            qCritical() << "Valid ID required for --status";
            return 1;
        }
        QVariantMap download = database.getDownload(id);
        if (download.isEmpty()) {
            qCritical() << "Download not found";
            return 1;
        }
        qInfo() << QString("ID: %1").arg(id);
        qInfo() << QString("URL: %1").arg(download["url"].toString());
        qInfo() << QString("Status: %1").arg(download["status"].toString());
        qInfo() << QString("Progress: %1%").arg(download["progress"].toDouble() * 100);
        qInfo() << QString("Downloaded: %1 bytes").arg(download["downloaded_size"].toLongLong());
    } else if (parser.isSet(historyOption)) {
        QVariantList history = database.getDownloadHistory(10, 0);
        qInfo() << "Recent download history:";
        for (const QVariant &variant : history) {
            QVariantMap entry = variant.toMap();
            qInfo() << QString("URL: %1, Completed: %2, Size: %3 bytes")
                       .arg(entry["url"].toString())
                       .arg(entry["completed_at"].toString())
                       .arg(entry["size"].toLongLong());
        }
    } else {
        parser.showHelp();
        return 0;
    }

    return app.exec();
}