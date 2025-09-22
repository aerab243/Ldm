#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QObject>
#include <QString>
#include <QByteArray>

class Encryption : public QObject
{
    Q_OBJECT

public:
    explicit Encryption(QObject *parent = nullptr);
    ~Encryption();

    // Encryption methods
    QByteArray encryptData(const QByteArray &data, const QString &password);
    QByteArray decryptData(const QByteArray &encryptedData, const QString &password);

    // File encryption
    bool encryptFile(const QString &inputPath, const QString &outputPath, const QString &password);
    bool decryptFile(const QString &inputPath, const QString &outputPath, const QString &password);

    // Utility methods
    QString generateKey(const QString &password, const QByteArray &salt = QByteArray());
    QByteArray generateSalt(int length = 32);

signals:
    void encryptionProgress(int percentage);
    void encryptionFinished(bool success, const QString &errorMessage);
    void decryptionFinished(bool success, const QString &errorMessage);

private:
    QByteArray deriveKey(const QString &password, const QByteArray &salt);
    QByteArray aesEncrypt(const QByteArray &data, const QByteArray &key, const QByteArray &iv);
    QByteArray aesDecrypt(const QByteArray &encryptedData, const QByteArray &key, const QByteArray &iv);
};

#endif // ENCRYPTION_H