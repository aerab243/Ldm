#ifndef METADATACACHE_H
#define METADATACACHE_H

#include <QObject>
#include <QCache>
#include <QVariantMap>
#include <QString>
#include <QReadWriteLock>

class MetadataCache : public QObject
{
    Q_OBJECT

public:
    explicit MetadataCache(QObject *parent = nullptr);
    ~MetadataCache();

    // Cache operations
    void store(const QString &key, const QVariantMap &metadata);
    QVariantMap retrieve(const QString &key) const;
    bool contains(const QString &key) const;
    void remove(const QString &key);
    void clear();

    // Configuration
    void setMaxCost(int cost);
    int maxCost() const;
    int size() const;

    // Persistence
    bool saveToFile(const QString &filePath) const;
    bool loadFromFile(const QString &filePath);

signals:
    void cacheHit(const QString &key);
    void cacheMiss(const QString &key);
    void cacheCleared();

private:
    QCache<QString, QVariantMap> *m_cache;
    mutable QReadWriteLock m_lock;

    // Helper to create cache entry
    QVariantMap *createCacheEntry(const QVariantMap &metadata) const;
};

#endif // METADATACACHE_H