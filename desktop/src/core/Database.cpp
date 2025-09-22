#include "Database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QDir>

Database::Database(QObject *parent)
    : QObject(parent)
{
}

Database::~Database()
{
    close();
}

bool Database::open(const QString &databasePath)
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(databasePath);

    if (!m_database.open()) {
        emit databaseError(m_database.lastError().text());
        return false;
    }

    if (!createTables()) {
        close();
        return false;
    }

    return true;
}

void Database::close()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool Database::isOpen() const
{
    return m_database.isOpen();
}

bool Database::createTables()
{
    QStringList queries = {
        "CREATE TABLE IF NOT EXISTS downloads ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "url TEXT NOT NULL,"
        "filename TEXT,"
        "filepath TEXT,"
        "status TEXT NOT NULL CHECK (status IN ('queued', 'downloading', 'paused', 'completed', 'failed', 'cancelled')),"
        "progress REAL CHECK (progress >= 0.0 AND progress <= 1.0),"
        "total_size INTEGER,"
        "downloaded_size INTEGER DEFAULT 0,"
        "speed INTEGER,"
        "eta INTEGER,"
        "error_message TEXT,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "started_at DATETIME,"
        "completed_at DATETIME,"
        "category_id INTEGER,"
        "checksum TEXT,"
        "checksum_type TEXT,"
        "priority INTEGER DEFAULT 1,"
        "segments INTEGER DEFAULT 1 CHECK (segments >= 1 AND segments <= 32),"
        "referrer TEXT,"
        "user_agent TEXT,"
        "authentication TEXT,"
        "proxy TEXT,"
        "resume_supported BOOLEAN DEFAULT 1,"
        "antivirus_scanned BOOLEAN DEFAULT 0,"
        "antivirus_result TEXT,"
        "encrypted BOOLEAN DEFAULT 0,"
        "metadata TEXT,"
        "FOREIGN KEY (category_id) REFERENCES categories(id)"
        ")",
        "CREATE TABLE IF NOT EXISTS categories ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE,"
        "description TEXT,"
        "default_path TEXT,"
        "color TEXT,"
        "icon TEXT,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")",
        "CREATE TABLE IF NOT EXISTS download_segments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "download_id INTEGER NOT NULL,"
        "segment_index INTEGER NOT NULL,"
        "start_offset INTEGER NOT NULL,"
        "end_offset INTEGER NOT NULL,"
        "downloaded_size INTEGER DEFAULT 0,"
        "status TEXT DEFAULT 'pending',"
        "FOREIGN KEY (download_id) REFERENCES downloads(id) ON DELETE CASCADE"
        ")",
        "CREATE TABLE IF NOT EXISTS download_history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "download_id INTEGER,"
        "url TEXT,"
        "filename TEXT,"
        "filepath TEXT,"
        "size INTEGER,"
        "completed_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "duration INTEGER,"
        "average_speed INTEGER,"
        "category_name TEXT,"
        "success BOOLEAN DEFAULT 1,"
        "FOREIGN KEY (download_id) REFERENCES downloads(id)"
        ")",
        "CREATE TABLE IF NOT EXISTS settings ("
        "key TEXT PRIMARY KEY,"
        "value TEXT,"
        "type TEXT CHECK (type IN ('string', 'int', 'bool', 'json')),"
        "category TEXT,"
        "description TEXT"
        ")",
        "CREATE TABLE IF NOT EXISTS browser_extensions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "browser TEXT NOT NULL,"
        "enabled BOOLEAN DEFAULT 1,"
        "settings TEXT"
        ")",
        "CREATE TABLE IF NOT EXISTS plugins ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE,"
        "version TEXT,"
        "type TEXT,"
        "enabled BOOLEAN DEFAULT 1,"
        "settings TEXT,"
        "installed_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")",
        "CREATE TABLE IF NOT EXISTS password_entries ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "domain TEXT NOT NULL,"
        "username TEXT,"
        "password TEXT NOT NULL,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "last_used DATETIME"
        ")",
        "CREATE TABLE IF NOT EXISTS favorite_sites ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT,"
        "url TEXT NOT NULL,"
        "category TEXT,"
        "added_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")",
        "CREATE INDEX IF NOT EXISTS idx_downloads_status ON downloads(status)",
        "CREATE INDEX IF NOT EXISTS idx_downloads_category ON downloads(category_id)",
        "CREATE INDEX IF NOT EXISTS idx_downloads_created ON downloads(created_at)",
        "CREATE INDEX IF NOT EXISTS idx_history_completed ON download_history(completed_at)",
        "CREATE INDEX IF NOT EXISTS idx_segments_download ON download_segments(download_id)",
        "CREATE VIRTUAL TABLE IF NOT EXISTS downloads_fts USING fts5(url, filename, filepath, content=downloads)"
    };

    for (const QString &query : queries) {
        if (!executeQuery(query)) {
            return false;
        }
    }

    return true;
}

int Database::insertDownload(const QVariantMap &downloadData)
{
    QSqlQuery q(m_database);
    q.prepare("INSERT INTO downloads (url, filename, filepath, status, progress, total_size, "
              "downloaded_size, speed, eta, error_message, started_at, completed_at, category_id, "
              "checksum, checksum_type, priority, segments, referrer, user_agent, authentication, "
              "proxy, resume_supported, antivirus_scanned, antivirus_result, encrypted, metadata) "
              "VALUES (:url, :filename, :filepath, :status, :progress, :total_size, "
              ":downloaded_size, :speed, :eta, :error_message, :started_at, :completed_at, :category_id, "
              ":checksum, :checksum_type, :priority, :segments, :referrer, :user_agent, :authentication, "
              ":proxy, :resume_supported, :antivirus_scanned, :antivirus_result, :encrypted, :metadata)");

    for (auto it = downloadData.begin(); it != downloadData.end(); ++it) {
        q.bindValue(":" + it.key(), it.value());
    }

    if (!q.exec()) {
        emit databaseError(q.lastError().text());
        return -1;
    }

    return q.lastInsertId().toInt();
}

bool Database::updateDownload(int id, const QVariantMap &downloadData)
{
    QString query = "UPDATE downloads SET url=:url, filename=:filename, filepath=:filepath, status=:status, "
                    "progress=:progress, total_size=:total_size, downloaded_size=:downloaded_size, speed=:speed, "
                    "eta=:eta, error_message=:error_message, started_at=:started_at, completed_at=:completed_at, "
                    "category_id=:category_id, checksum=:checksum, checksum_type=:checksum_type, priority=:priority, "
                    "segments=:segments, referrer=:referrer, user_agent=:user_agent, authentication=:authentication, "
                    "proxy=:proxy, resume_supported=:resume_supported, antivirus_scanned=:antivirus_scanned, "
                    "antivirus_result=:antivirus_result, encrypted=:encrypted, metadata=:metadata WHERE id=:id";

    QVariantMap params = downloadData;
    params["id"] = id;

    return executeQuery(query, params);
}

bool Database::deleteDownload(int id)
{
    return executeQuery("DELETE FROM downloads WHERE id=:id", {{"id", id}});
}

QVariantMap Database::getDownload(int id)
{
    return executeSingleRowQuery("SELECT * FROM downloads WHERE id=:id", {{"id", id}});
}

QVariantList Database::getDownloads(const QString &status)
{
    QString query = "SELECT * FROM downloads";
    QVariantMap params;
    if (!status.isEmpty()) {
        query += " WHERE status=:status";
        params["status"] = status;
    }
    query += " ORDER BY created_at DESC";
    return executeSelectQuery(query, params);
}

QVariantList Database::getDownloadsByCategory(int categoryId)
{
    return executeSelectQuery("SELECT * FROM downloads WHERE category_id=:category_id ORDER BY created_at DESC",
                              {{"category_id", categoryId}});
}

bool Database::insertCategory(const QVariantMap &categoryData)
{
    QString query = "INSERT INTO categories (name, description, default_path, color, icon) "
                    "VALUES (:name, :description, :default_path, :color, :icon)";
    return executeQuery(query, categoryData);
}

bool Database::updateCategory(int id, const QVariantMap &categoryData)
{
    QString query = "UPDATE categories SET name=:name, description=:description, default_path=:default_path, "
                    "color=:color, icon=:icon, updated_at=CURRENT_TIMESTAMP WHERE id=:id";
    QVariantMap params = categoryData;
    params["id"] = id;
    return executeQuery(query, params);
}

bool Database::deleteCategory(int id)
{
    return executeQuery("DELETE FROM categories WHERE id=:id", {{"id", id}});
}

QVariantMap Database::getCategory(int id)
{
    return executeSingleRowQuery("SELECT * FROM categories WHERE id=:id", {{"id", id}});
}

QVariantList Database::getCategories()
{
    return executeSelectQuery("SELECT * FROM categories ORDER BY name");
}

bool Database::setSetting(const QString &key, const QVariant &value, const QString &type)
{
    QString query = "INSERT OR REPLACE INTO settings (key, value, type) VALUES (:key, :value, :type)";
    return executeQuery(query, {{"key", key}, {"value", value}, {"type", type}});
}

QVariant Database::getSetting(const QString &key)
{
    QVariantMap result = executeSingleRowQuery("SELECT value FROM settings WHERE key=:key", {{"key", key}});
    return result.value("value");
}

QVariantList Database::getSettings(const QString &category)
{
    QString query = "SELECT * FROM settings";
    QVariantMap params;
    if (!category.isEmpty()) {
        query += " WHERE category=:category";
        params["category"] = category;
    }
    return executeSelectQuery(query, params);
}

bool Database::insertDownloadHistory(const QVariantMap &historyData)
{
    QString query = "INSERT INTO download_history (download_id, url, filename, filepath, size, duration, "
                    "average_speed, category_name, success) VALUES (:download_id, :url, :filename, :filepath, "
                    ":size, :duration, :average_speed, :category_name, :success)";
    return executeQuery(query, historyData);
}

QVariantList Database::getDownloadHistory(int limit, int offset)
{
    QString query = "SELECT * FROM download_history ORDER BY completed_at DESC LIMIT :limit OFFSET :offset";
    return executeSelectQuery(query, {{"limit", limit}, {"offset", offset}});
}

bool Database::executeQuery(const QString &query, const QVariantMap &params)
{
    QSqlQuery q;
    q.prepare(query);
    for (auto it = params.begin(); it != params.end(); ++it) {
        q.bindValue(":" + it.key(), it.value());
    }
    if (!q.exec()) {
        emit databaseError(q.lastError().text());
        return false;
    }
    return true;
}

QVariantList Database::executeSelectQuery(const QString &query, const QVariantMap &params)
{
    QSqlQuery q;
    q.prepare(query);
    for (auto it = params.begin(); it != params.end(); ++it) {
        q.bindValue(":" + it.key(), it.value());
    }
    if (!q.exec()) {
        emit databaseError(q.lastError().text());
        return QVariantList();
    }

    QVariantList results;
    while (q.next()) {
        QVariantMap row;
        QSqlRecord record = q.record();
        for (int i = 0; i < record.count(); ++i) {
            row[record.fieldName(i)] = q.value(i);
        }
        results.append(row);
    }
    return results;
}

QVariantMap Database::executeSingleRowQuery(const QString &query, const QVariantMap &params)
{
    QVariantList results = executeSelectQuery(query, params);
    if (results.isEmpty()) {
        return QVariantMap();
    }
    return results.first().toMap();
}