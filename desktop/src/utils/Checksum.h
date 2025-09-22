#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

class Checksum
{
public:
    Checksum();

    // File-based checksums
    static QString md5(const QString &filePath);
    static QString sha256(const QString &filePath);

    // Data-based checksums
    static QString md5(const QByteArray &data);
    static QString sha256(const QByteArray &data);

    // Verify checksum
    static bool verifyMd5(const QString &filePath, const QString &expectedMd5);
    static bool verifySha256(const QString &filePath, const QString &expectedSha256);

private:
    static QString calculateHash(const QString &filePath, QCryptographicHash::Algorithm algorithm);
    static QString calculateHash(const QByteArray &data, QCryptographicHash::Algorithm algorithm);
};

#endif // CHECKSUM_H