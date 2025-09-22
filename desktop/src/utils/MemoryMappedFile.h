
#ifndef MEMORYMAPPEDFILE_H
#define MEMORYMAPPEDFILE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QFile>

class MemoryMappedFile : public QObject
{
    Q_OBJECT

public:
    explicit MemoryMappedFile(QObject *parent = nullptr);
    ~MemoryMappedFile();

    // File operations
    bool open(const QString &filePath, QFile::OpenMode mode = QFile::ReadOnly);
    void close();
    bool isOpen() const;

    // Mapping operations
    bool map(qint64 offset = 0, qint64 size = -1);
    void unmap();
    bool isMapped() const;

    // Data access
    const uchar *data() const;
    qint64 size() const;
    qint64 mappedSize() const;

    // Utility methods
    bool resize(qint64 newSize);
    bool flush(qint64 offset = 0, qint64 size = -1);

    // Error handling
    QString errorString() const;

signals:
    void mappingChanged(bool mapped);

private:
    QFile *m_file;
    uchar *m_mappedData;
    qint64 m_mappedSize;
    QString m_errorString;

    void setError(const QString &error);
};

#endif // MEMORYMAPPEDFILE_H