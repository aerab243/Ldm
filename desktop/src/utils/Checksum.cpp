#include "Checksum.h"
#include <QFile>
#include <QDebug>

Checksum::Checksum()
{
}

QString Checksum::md5(const QString &filePath)
{
    return calculateHash(filePath, QCryptographicHash::Md5);
}

QString Checksum::sha256(const QString &filePath)
{
    return calculateHash(filePath, QCryptographicHash::Sha256);
}

QString Checksum::md5(const QByteArray &data)
{
    return calculateHash(data, QCryptographicHash::Md5);
}

QString Checksum::sha256(const QByteArray &data)
{
    return calculateHash(data, QCryptographicHash::Sha256);
}

bool Checksum::verifyMd5(const QString &filePath, const QString &expectedMd5)
{
    QString actualMd5 = md5(filePath);
    return actualMd5.compare(expectedMd5, Qt::CaseInsensitive) == 0;
}

bool Checksum::verifySha256(const QString &filePath, const QString &expectedSha256)
{
    QString actualSha256 = sha256(filePath);
    return actualSha256.compare(expectedSha256, Qt::CaseInsensitive) == 0;
}

QString Checksum::calculateHash(const QString &filePath, QCryptographicHash::Algorithm algorithm)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for checksum:" << filePath;
        return QString();
    }

    QCryptographicHash hash(algorithm);
    if (!hash.addData(&file)) {
        qWarning() << "Failed to calculate checksum for:" << filePath;
        return QString();
    }

    return hash.result().toHex();
}

QString Checksum::calculateHash(const QByteArray &data, QCryptographicHash::Algorithm algorithm)
{
    QCryptographicHash hash(algorithm);
    hash.addData(data);
    return hash.result().toHex();
}