#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QString>
#include <QSqlDatabase>
#include <QVariantList>
#include <QSqlRecord>
#include <QSqlQuery>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool open(const QString &databasePath);
    void close();
    bool isOpen() const;

    // Download operations
    int insertDownload(const QVariantMap &downloadData);
    bool updateDownload(int id, const QVariantMap &downloadData);
    bool deleteDownload(int id);
    QVariantMap getDownload(int id);
    QVariantList getDownloads(const QString &status = QString());
    QVariantList getDownloadsByCategory(int categoryId);

    // Category operations
    bool insertCategory(const QVariantMap &categoryData);
    bool updateCategory(int id, const QVariantMap &categoryData);
    bool deleteCategory(int id);
    QVariantMap getCategory(int id);
    QVariantList getCategories();

    // Settings operations
    bool setSetting(const QString &key, const QVariant &value, const QString &type = "string");
    QVariant getSetting(const QString &key);
    QVariantList getSettings(const QString &category = QString());

    // History operations
    bool insertDownloadHistory(const QVariantMap &historyData);
    QVariantList getDownloadHistory(int limit = 100, int offset = 0);

    // Initialization
    bool createTables();

signals:
    void databaseError(const QString &error);

private:
    QSqlDatabase m_database;
    bool executeQuery(const QString &query, const QVariantMap &params = QVariantMap());
    QVariantList executeSelectQuery(const QString &query, const QVariantMap &params = QVariantMap());
    QVariantMap executeSingleRowQuery(const QString &query, const QVariantMap &params = QVariantMap());
};

#endif // DATABASE_H