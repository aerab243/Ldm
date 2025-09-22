#include "Logger.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QCoreApplication>

Logger* Logger::m_instance = nullptr;
QMutex Logger::m_mutex;

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_logLevel(Info)
    , m_consoleOutput(true)
{
}

Logger::~Logger()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

Logger* Logger::instance()
{
    QMutexLocker locker(&m_mutex);
    if (!m_instance) {
        m_instance = new Logger();
    }
    return m_instance;
}

void Logger::destroyInstance()
{
    QMutexLocker locker(&m_mutex);
    if (m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

void Logger::setLogFile(const QString &filename)
{
    QMutexLocker locker(&m_fileMutex);
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
    
    m_logFile.setFileName(filename);
    if (!m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << filename;
    }
}

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::enableConsoleOutput(bool enable)
{
    m_consoleOutput = enable;
}

void Logger::log(LogLevel level, const QString &message, const QString &category)
{
    if (level < m_logLevel) {
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2]").arg(timestamp, levelStr);
    
    if (!category.isEmpty()) {
        logMessage += QString(" [%1]").arg(category);
    }
    
    logMessage += QString(": %1").arg(message);

    // Write to file
    {
        QMutexLocker locker(&m_fileMutex);
        if (m_logFile.isOpen()) {
            m_textStream << logMessage << "\n";
            m_textStream.flush();
        }
    }

    // Write to console
    if (m_consoleOutput) {
        switch (level) {
        case Debug:
        case Info:
            qDebug().noquote() << logMessage;
            break;
        case Warning:
            qWarning().noquote() << logMessage;
            break;
        case Error:
        case Critical:
            qCritical().noquote() << logMessage;
            break;
        }
    }

    // Emit signal
    emit logEntry(level, message, category);
}

void Logger::debug(const QString &message, const QString &category)
{
    log(Debug, message, category);
}

void Logger::info(const QString &message, const QString &category)
{
    log(Info, message, category);
}

void Logger::warning(const QString &message, const QString &category)
{
    log(Warning, message, category);
}

void Logger::error(const QString &message, const QString &category)
{
    log(Error, message, category);
}

void Logger::critical(const QString &message, const QString &category)
{
    log(Critical, message, category);
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
    case Debug: return "DEBUG";
    case Info: return "INFO";
    case Warning: return "WARNING";
    case Error: return "ERROR";
    case Critical: return "CRITICAL";
    default: return "UNKNOWN";
    }
}