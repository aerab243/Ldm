#include "ClamAVScanner.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QRegularExpression>

ClamAVScanner::ClamAVScanner(QObject *parent)
    : QObject(parent)
    , m_clamProcess(new QProcess(this))
    , m_clamAVPath(findClamAVExecutable())
{
    connect(m_clamProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ClamAVScanner::onProcessFinished);
    connect(m_clamProcess, &QProcess::errorOccurred,
            this, &ClamAVScanner::onProcessError);
}

ClamAVScanner::~ClamAVScanner()
{
    if (m_clamProcess->state() != QProcess::NotRunning) {
        m_clamProcess->kill();
        m_clamProcess->waitForFinished(3000);
    }
}

ClamAVScanner::ScanResult ClamAVScanner::scanFile(const QString &filePath)
{
    if (m_clamAVPath.isEmpty()) {
        m_lastError = "ClamAV executable not found";
        return Error;
    }

    if (m_clamProcess->state() != QProcess::NotRunning) {
        m_lastError = "Scanner already running";
        return Error;
    }

    m_currentFile = filePath;
    m_clamProcess->start(m_clamAVPath, QStringList() << "--no-summary" << filePath);

    if (!m_clamProcess->waitForFinished(30000)) { // 30 second timeout
        m_clamProcess->kill();
        m_lastError = "Scan timeout";
        return Error;
    }

    return parseScanResult(QString::fromUtf8(m_clamProcess->readAllStandardOutput()));
}

ClamAVScanner::ScanResult ClamAVScanner::scanData(const QByteArray &data)
{
    if (m_clamAVPath.isEmpty()) {
        m_lastError = "ClamAV executable not found";
        return Error;
    }

    // Create temporary file for scanning
    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        m_lastError = "Failed to create temporary file";
        return Error;
    }

    tempFile.write(data);
    tempFile.close();

    ScanResult result = scanFile(tempFile.fileName());
    tempFile.remove();

    return result;
}

void ClamAVScanner::setClamAVPath(const QString &path)
{
    m_clamAVPath = path;
}

QString ClamAVScanner::clamAVPath() const
{
    return m_clamAVPath;
}

bool ClamAVScanner::updateDatabase()
{
    if (m_clamAVPath.isEmpty()) {
        m_lastError = "ClamAV executable not found";
        return false;
    }

    QProcess updateProcess;
    updateProcess.start("freshclam", QStringList());
    
    if (!updateProcess.waitForFinished(120000)) { // 2 minute timeout
        m_lastError = "Database update timeout";
        return false;
    }

    return updateProcess.exitCode() == 0;
}

QString ClamAVScanner::databaseVersion() const
{
    if (m_clamAVPath.isEmpty()) {
        return QString();
    }

    QProcess versionProcess;
    versionProcess.start(m_clamAVPath, QStringList() << "--version");
    
    if (!versionProcess.waitForFinished(5000)) {
        return QString();
    }

    QString output = QString::fromUtf8(versionProcess.readAllStandardOutput());
    QRegularExpression regex("ClamAV (\\d+\\.\\d+\\.\\d+)");
    QRegularExpressionMatch match = regex.match(output);
    
    if (match.hasMatch()) {
        return match.captured(1);
    }
    
    return QString();
}

QString ClamAVScanner::lastError() const
{
    return m_lastError;
}

void ClamAVScanner::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit) {
        m_lastError = "ClamAV process crashed";
        emit scanCompleted(m_currentFile, Error, m_lastError);
        return;
    }

    QString output = QString::fromUtf8(m_clamProcess->readAllStandardOutput());
    ScanResult result = parseScanResult(output);
    
    QString details;
    if (result == Infected) {
        // Extract virus name from output
        QRegularExpression virusRegex(": (.+) FOUND");
        QRegularExpressionMatch virusMatch = virusRegex.match(output);
        if (virusMatch.hasMatch()) {
            details = virusMatch.captured(1);
        }
    }
    
    emit scanCompleted(m_currentFile, result, details);
}

void ClamAVScanner::onProcessError(QProcess::ProcessError error)
{
    switch (error) {
    case QProcess::FailedToStart:
        m_lastError = "Failed to start ClamAV process";
        break;
    case QProcess::Crashed:
        m_lastError = "ClamAV process crashed";
        break;
    case QProcess::Timedout:
        m_lastError = "ClamAV process timed out";
        break;
    case QProcess::WriteError:
        m_lastError = "Write error to ClamAV process";
        break;
    case QProcess::ReadError:
        m_lastError = "Read error from ClamAV process";
        break;
    default:
        m_lastError = "Unknown ClamAV process error";
    }
    
    emit scanCompleted(m_currentFile, Error, m_lastError);
}

ClamAVScanner::ScanResult ClamAVScanner::parseScanResult(const QString &output)
{
    if (output.contains("FOUND")) {
        return Infected;
    } else if (output.contains("OK")) {
        return Clean;
    } else {
        m_lastError = "Failed to parse scan result";
        return Error;
    }
}

QString ClamAVScanner::findClamAVExecutable()
{
    QStringList possiblePaths = {
        "/usr/bin/clamscan",
        "/usr/local/bin/clamscan",
        "/opt/local/bin/clamscan",
        "C:/Program Files/ClamAV/clamscan.exe",
        "C:/Program Files (x86)/ClamAV/clamscan.exe"
    };
    
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }
    
    // Try to find in PATH
    QProcess whichProcess;
    whichProcess.start("which", QStringList() << "clamscan");
    if (whichProcess.waitForFinished(2000) && whichProcess.exitCode() == 0) {
        QString path = QString::fromUtf8(whichProcess.readAllStandardOutput()).trimmed();
        if (!path.isEmpty()) {
            return path;
        }
    }
    
    return QString();
}