#include "Encryption.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QCryptographicHash>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

Encryption::Encryption(QObject *parent)
    : QObject(parent)
{
}

Encryption::~Encryption()
{
}

QByteArray Encryption::encryptData(const QByteArray &data, const QString &password)
{
    QByteArray salt = generateSalt();
    QByteArray key = deriveKey(password, salt);
    QByteArray iv = generateSalt(16); // AES block size

    QByteArray encrypted = aesEncrypt(data, key, iv);

    // Prepend salt and IV
    QByteArray result;
    result.append("AES256"); // Magic
    result.append(salt);
    result.append(iv);
    result.append(encrypted);

    return result;
}

QByteArray Encryption::decryptData(const QByteArray &encryptedData, const QString &password)
{
    if (encryptedData.left(6) != "AES256") {
        qWarning() << "Invalid encrypted data format";
        return QByteArray();
    }

    QByteArray data = encryptedData.mid(6);
    QByteArray salt = data.left(32);
    data = data.mid(32);
    QByteArray iv = data.left(16);
    data = data.mid(16);

    QByteArray key = deriveKey(password, salt);
    return aesDecrypt(data, key, iv);
}

bool Encryption::encryptFile(const QString &inputPath, const QString &outputPath, const QString &password)
{
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        emit encryptionFinished(false, "Failed to open input file");
        return false;
    }

    QByteArray data = inputFile.readAll();
    inputFile.close();

    QByteArray encrypted = encryptData(data, password);

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        emit encryptionFinished(false, "Failed to open output file");
        return false;
    }

    outputFile.write(encrypted);
    outputFile.close();

    emit encryptionFinished(true, QString());
    return true;
}

bool Encryption::decryptFile(const QString &inputPath, const QString &outputPath, const QString &password)
{
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        emit decryptionFinished(false, "Failed to open input file");
        return false;
    }

    QByteArray encryptedData = inputFile.readAll();
    inputFile.close();

    QByteArray decrypted = decryptData(encryptedData, password);

    if (decrypted.isEmpty()) {
        emit decryptionFinished(false, "Decryption failed");
        return false;
    }

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        emit decryptionFinished(false, "Failed to open output file");
        return false;
    }

    outputFile.write(decrypted);
    outputFile.close();

    emit decryptionFinished(true, QString());
    return true;
}

QString Encryption::generateKey(const QString &password, const QByteArray &salt)
{
    return QString::fromUtf8(deriveKey(password, salt).toHex());
}

QByteArray Encryption::generateSalt(int length)
{
    QByteArray salt(length, 0);
    RAND_bytes(reinterpret_cast<unsigned char *>(salt.data()), length);
    return salt;
}

QByteArray Encryption::deriveKey(const QString &password, const QByteArray &salt)
{
    QByteArray key(32, 0); // 256-bit key
    PKCS5_PBKDF2_HMAC(password.toUtf8().constData(), password.length(),
                      reinterpret_cast<const unsigned char *>(salt.constData()), salt.length(),
                      10000, EVP_sha256(),
                      32, reinterpret_cast<unsigned char *>(key.data()));
    return key;
}

QByteArray Encryption::aesEncrypt(const QByteArray &data, const QByteArray &key, const QByteArray &iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char *>(key.constData()),
                       reinterpret_cast<const unsigned char *>(iv.constData()));

    QByteArray encrypted(data.size() + AES_BLOCK_SIZE, 0);
    int len = 0;
    int encryptedLen = 0;

    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(encrypted.data()), &len,
                      reinterpret_cast<const unsigned char *>(data.constData()), data.size());
    encryptedLen += len;

    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(encrypted.data() + encryptedLen), &len);
    encryptedLen += len;

    EVP_CIPHER_CTX_free(ctx);

    encrypted.resize(encryptedLen);
    return encrypted;
}

QByteArray Encryption::aesDecrypt(const QByteArray &encryptedData, const QByteArray &key, const QByteArray &iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char *>(key.constData()),
                       reinterpret_cast<const unsigned char *>(iv.constData()));

    QByteArray decrypted(encryptedData.size(), 0);
    int len = 0;
    int decryptedLen = 0;

    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(decrypted.data()), &len,
                      reinterpret_cast<const unsigned char *>(encryptedData.constData()), encryptedData.size());
    decryptedLen += len;

    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(decrypted.data() + decryptedLen), &len);
    decryptedLen += len;

    EVP_CIPHER_CTX_free(ctx);

    decrypted.resize(decryptedLen);
    return decrypted;
}