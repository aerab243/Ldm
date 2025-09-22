#include "MemoryMappedFile.h"
#include <QDebug>

MemoryMappedFile::MemoryMappedFile(QObject *parent)
    : QObject(parent)
    , m_file(new QFile(this))
    , m_mappedData(nullptr)
    , m_mappedSize(0)
{
}

MemoryMappedFile::~MemoryMappedFile()
{
    unmap();
    if (m_file->isOpen()) {
        m_file->close();
    }
}

bool MemoryMappedFile::open(const QString &filePath, QFile::OpenMode mode)
{
    if (m_file->isOpen()) {
        setError("File already open");
        return false;
    }

    m_file->setFileName(filePath);
    if (!m_file->open(mode)) {
        setError(m_file->errorString());
        return false;
    }

    return true;
}

void MemoryMappedFile::close()
{
    unmap();
    m_file->close();
}

bool MemoryMappedFile::isOpen() const
{
    return m_file->isOpen();
}

bool MemoryMappedFile::map(qint64 offset, qint64 size)
{
    if (!m_file->isOpen()) {
        setError("File not open");
        return false;
    }

    if (isMapped()) {
        unmap();
    }

    qint64 fileSize = m_file->size();
    if (offset >= fileSize) {
        setError("Offset beyond file size");
        return false;
    }

    qint64 mapSize = (size == -1) ? (fileSize - offset) : size;
    if (mapSize <= 0) {
        setError("Invalid mapping size");
        return false;
    }

    m_mappedData = m_file->map(offset, mapSize);
    if (!m_mappedData) {
        setError("Failed to map file");
        return false;
    }

    m_mappedSize = mapSize;
    emit mappingChanged(true);
    return true;
}

void MemoryMappedFile::unmap()
{
    if (isMapped()) {
        m_file->unmap(m_mappedData);
        m_mappedData = nullptr;
        m_mappedSize = 0;
        emit mappingChanged(false);
    }
}

bool MemoryMappedFile::isMapped() const
{
    return m_mappedData != nullptr;
}

const uchar *MemoryMappedFile::data() const
{
    return m_mappedData;
}

qint64 MemoryMappedFile::size() const
{
    return m_file->size();
}

qint64 MemoryMappedFile::mappedSize() const
{
    return m_mappedSize;
}

bool MemoryMappedFile::resize(qint64 newSize)
{
    if (!m_file->isOpen()) {
        setError("File not open");
        return false;
    }

    unmap();
    if (!m_file->resize(newSize)) {
        setError(m_file->errorString());
        return false;
    }

    return true;
}

bool MemoryMappedFile::flush(qint64 offset, qint64 size)
{
    if (!isMapped()) {
        setError("File not mapped");
        return false;
    }

    qint64 flushSize = (size == -1) ? m_mappedSize : size;
    if (!m_file->flush()) {
        setError("Failed to flush mapped data");
        return false;
    }

    return true;
}

QString MemoryMappedFile::errorString() const
{
    return m_errorString;
}

void MemoryMappedFile::setError(const QString &error)
{
    m_errorString = error;
    qWarning() << "MemoryMappedFile error:" << error;
}