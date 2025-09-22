#include "MetadataCache.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

MetadataCache::MetadataCache(QObject *parent)
    : QObject(parent)
    , m_cache(new QCache<QString, QVariantMap>)
{
    // Set default max cost (number of entries)
    m_cache->setMaxCost(1000);
}

MetadataCache::~MetadataCache()
{
    delete m_cache;
}

void MetadataCache::store(const QString &key, const QVariantMap &metadata)
{
    QWriteLocker locker(&m_lock);
    QVariantMap *entry = createCacheEntry(metadata);
    m_cache->insert(key, entry, 1); // Cost of 1 per entry
}

QVariantMap MetadataCache::retrieve(const QString &key) const
{
    QReadLocker locker(&m_lock);
    QVariantMap *entry = m_cache->object(key);
    if (entry) {
        emit const_cast<MetadataCache*>(this)->cacheHit(key);
        return *entry;
    } else {
        emit const_cast<MetadataCache*>(this)->cacheMiss(key);
        return QVariantMap();
    }
}

bool MetadataCache::contains(const QString &key) const
{
    QReadLocker locker(&m_lock);
    return m_cache->contains(key);
}

void MetadataCache::remove(const QString &key)
{
    QWriteLocker locker(&m_lock);
    m_cache->remove(key);
}

void MetadataCache::clear()
{
    QWriteLocker locker(&m_lock);
    m_cache->clear();
    emit cacheCleared();
}

void MetadataCache::setMaxCost(int cost)
{
    QWriteLocker locker(&m_lock);
    m_cache->setMaxCost(cost);
}

int MetadataCache::maxCost() const
{
    QReadLocker locker(&m_lock);
    return m_cache->maxCost();
}

int MetadataCache::size() const
{
    QReadLocker locker(&m_lock);
    return m_cache->size();
}

bool MetadataCache::saveToFile(const QString &filePath) const
{
    QReadLocker locker(&m_lock);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open cache file for writing:" << filePath;
        return false;
    }

    QDataStream stream(&file);
    
    // Convert QCache to QMap for serialization
    QMap<QString, QMap<QString, QVariant>> cacheData;
    QStringList keys = m_cache->keys();
    for (const QString &key : keys) {
        QMap<QString, QVariant> *value = m_cache->object(key);
        if (value) {
            cacheData[key] = *value;
        }
    }
    
    stream << cacheData;
    
    file.close();
    return true;
}

bool MetadataCache::loadFromFile(const QString &filePath)
{
    QWriteLocker locker(&m_lock);
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open cache file for reading:" << filePath;
        return false;
    }

    QDataStream stream(&file);
    
    // Load data into QMap and convert to QCache
    QMap<QString, QMap<QString, QVariant>> cacheData;
    stream >> cacheData;
    
    // Clear existing cache and populate from loaded data
    m_cache->clear();
    for (auto it = cacheData.constBegin(); it != cacheData.constEnd(); ++it) {
        m_cache->insert(it.key(), new QMap<QString, QVariant>(it.value()));
    }
    
    file.close();
    return true;
}

QVariantMap *MetadataCache::createCacheEntry(const QVariantMap &metadata) const
{
    return new QVariantMap(metadata);
}