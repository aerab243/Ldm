#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

class Logger : public QObject
{
    Q_OBJECT

public:
    enum LogLevel {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    Q_ENUM(LogLevel)

    static Logger* instance();
    static void destroyInstance();

    void setLogFile(const QString &filename);
    void setLogLevel(LogLevel level);
    void enableConsoleOutput(bool enable);

    void log(LogLevel level, const QString &message, const QString &category = QString());

    // Convenience methods
    void debug(const QString &message, const QString &category = QString());
    void info(const QString &message, const QString &category = QString());
    void warning(const QString &message, const QString &category = QString());
    void error(const QString &message, const QString &category = QString());
    void critical(const QString &message, const QString &category = QString());

signals:
    void logEntry(LogLevel level, const QString &message, const QString &category);

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

    static Logger* m_instance;
    static QMutex m_mutex;

    QFile m_logFile;
    QTextStream m_textStream;
    LogLevel m_logLevel;
    bool m_consoleOutput;
    QMutex m_fileMutex;

    QString levelToString(LogLevel level) const;
};

#endif // LOGGER_H