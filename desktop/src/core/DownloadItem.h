#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QObject>
#include <QString>
#include <QDateTime>

class DownloadItem : public QObject
{
    Q_OBJECT

public:
    explicit DownloadItem(QObject *parent = nullptr);
    DownloadItem(int id, const QString &url, const QString &filename, QObject *parent = nullptr);

    // Getters
    int getId() const { return m_id; }
    QString getUrl() const { return m_url; }
    QString getFilename() const { return m_filename; }
    QString getFileName() const { return m_filename; } // Alias for getFilename
    QString getFilepath() const { return m_filepath; }
    QString getStatus() const { return m_status; }
    double getProgress() const;
    qint64 getTotalSize() const { return m_totalSize; }
    qint64 getDownloadedSize() const { return m_downloadedSize; }
    int getSpeed() const { return m_speed; }
    int getEta() const { return m_eta; }
    QString getErrorMessage() const { return m_errorMessage; }
    QDateTime getCreatedAt() const { return m_createdAt; }
    QDateTime getStartedAt() const { return m_startedAt; }
    QDateTime getCompletedAt() const { return m_completedAt; }
    int getCategoryId() const { return m_categoryId; }
    QString getChecksum() const { return m_checksum; }
    QString getChecksumType() const { return m_checksumType; }
    int getPriority() const { return m_priority; }
    int getSegments() const { return m_segments; }
    QString getReferrer() const { return m_referrer; }
    QString getUserAgent() const { return m_userAgent; }
    QString getAuthentication() const { return m_authentication; }
    QString getProxy() const { return m_proxy; }
    bool getResumeSupported() const { return m_resumeSupported; }
    bool getAntivirusScanned() const { return m_antivirusScanned; }
    QString getAntivirusResult() const { return m_antivirusResult; }
    bool getEncrypted() const { return m_encrypted; }
    QString getMetadata() const { return m_metadata; }

    // Setters
    void setId(int id) { m_id = id; }
    void setUrl(const QString &url) { m_url = url; }
    void setFilename(const QString &filename) { m_filename = filename; }
    void setFilepath(const QString &filepath) { m_filepath = filepath; }
    void setStatus(const QString &status) { m_status = status; }
    void setTotalSize(qint64 size) { m_totalSize = size; }
    void setDownloadedSize(qint64 size) { m_downloadedSize = size; }
    void setSpeed(int speed) { m_speed = speed; }
    void setEta(int eta) { m_eta = eta; }
    void setErrorMessage(const QString &message) { m_errorMessage = message; }
    void setCreatedAt(const QDateTime &time) { m_createdAt = time; }
    void setStartedAt(const QDateTime &time) { m_startedAt = time; }
    void setCompletedAt(const QDateTime &time) { m_completedAt = time; }
    void setCategoryId(int id) { m_categoryId = id; }
    void setChecksum(const QString &checksum) { m_checksum = checksum; }
    void setChecksumType(const QString &type) { m_checksumType = type; }
    void setPriority(int priority) { m_priority = priority; }
    void setSegments(int segments) { m_segments = segments; }
    void setReferrer(const QString &referrer) { m_referrer = referrer; }
    void setUserAgent(const QString &agent) { m_userAgent = agent; }
    void setAuthentication(const QString &auth) { m_authentication = auth; }
    void setProxy(const QString &proxy) { m_proxy = proxy; }
    void setResumeSupported(bool supported) { m_resumeSupported = supported; }
    void setAntivirusScanned(bool scanned) { m_antivirusScanned = scanned; }
    void setAntivirusResult(const QString &result) { m_antivirusResult = result; }
    void setEncrypted(bool encrypted) { m_encrypted = encrypted; }
    void setMetadata(const QString &metadata) { m_metadata = metadata; }

signals:
    void progressChanged(double progress);
    void statusChanged(const QString &status);
    void downloadCompleted();

private:
    int m_id;
    QString m_url;
    QString m_filename;
    QString m_filepath;
    QString m_status;
    qint64 m_totalSize;
    qint64 m_downloadedSize;
    int m_speed;
    int m_eta;
    QString m_errorMessage;
    QDateTime m_createdAt;
    QDateTime m_startedAt;
    QDateTime m_completedAt;
    int m_categoryId;
    QString m_checksum;
    QString m_checksumType;
    int m_priority;
    int m_segments;
    QString m_referrer;
    QString m_userAgent;
    QString m_authentication;
    QString m_proxy;
    bool m_resumeSupported;
    bool m_antivirusScanned;
    QString m_antivirusResult;
    bool m_encrypted;
    QString m_metadata;
};

#endif // DOWNLOADITEM_H