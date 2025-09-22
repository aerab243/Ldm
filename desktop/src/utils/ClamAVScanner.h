#ifndef CLAMAVSCANNER_H
#define CLAMAVSCANNER_H

#include <QObject>
#include <QString>
#include <QProcess>

class ClamAVScanner : public QObject
{
    Q_OBJECT

public:
    enum ScanResult {
        Clean,
        Infected,
        Error
    };

    explicit ClamAVScanner(QObject *parent = nullptr);
    ~ClamAVScanner();

    // Scanning methods
    ScanResult scanFile(const QString &filePath);
    ScanResult scanData(const QByteArray &data);

    // Configuration
    void setClamAVPath(const QString &path);
    QString clamAVPath() const;

    // Database management
    bool updateDatabase();
    QString databaseVersion() const;

    // Error handling
    QString lastError() const;

signals:
    void scanProgress(int percentage);
    void scanCompleted(const QString &filePath, ScanResult result, const QString &details);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    QProcess *m_clamProcess;
    QString m_clamAVPath;
    QString m_lastError;
    QString m_currentFile;

    ScanResult parseScanResult(const QString &output);
    QString findClamAVExecutable();
};

#endif // CLAMAVSCANNER_H