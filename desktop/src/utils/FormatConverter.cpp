#include "FormatConverter.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QRegularExpression>

FormatConverter::FormatConverter(QObject *parent)
    : QObject(parent)
    , m_ffmpegProcess(new QProcess(this))
{
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &FormatConverter::onProcessFinished);
    connect(m_ffmpegProcess, &QProcess::errorOccurred,
            this, &FormatConverter::onProcessError);
}

FormatConverter::~FormatConverter()
{
    if (m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
        m_ffmpegProcess->waitForFinished(3000);
    }
}

bool FormatConverter::convertVideo(const QString &inputPath, const QString &outputPath, const QString &format, const QVariantMap &options)
{
    if (m_ffmpegProcess->state() != QProcess::NotRunning) {
        emit conversionError("FFmpeg process already running");
        return false;
    }

    m_currentInput = inputPath;
    m_currentOutput = outputPath;

    QString command = buildFfmpegCommand(inputPath, outputPath, format, options);
    m_ffmpegProcess->start("ffmpeg", command.split(' ', Qt::SkipEmptyParts));

    return true;
}

bool FormatConverter::convertAudio(const QString &inputPath, const QString &outputPath, const QString &format, const QVariantMap &options)
{
    QVariantMap audioOptions = options;
    audioOptions["audio_only"] = true;
    return convertVideo(inputPath, outputPath, format, audioOptions);
}

bool FormatConverter::extractAudio(const QString &inputPath, const QString &outputPath, const QString &format)
{
    QVariantMap options;
    options["audio_only"] = true;
    options["audio_codec"] = format == "mp3" ? "libmp3lame" : "aac";
    return convertVideo(inputPath, outputPath, format, options);
}

QStringList FormatConverter::supportedVideoFormats() const
{
    return QStringList() << "mp4" << "avi" << "mkv" << "mov" << "wmv" << "flv" << "webm";
}

QStringList FormatConverter::supportedAudioFormats() const
{
    return QStringList() << "mp3" << "aac" << "wav" << "flac" << "ogg";
}

QVariantMap FormatConverter::getMediaInfo(const QString &inputPath)
{
    QVariantMap info;
    // Placeholder - would use ffprobe to get media information
    info["duration"] = 0;
    info["width"] = 0;
    info["height"] = 0;
    info["bitrate"] = 0;
    return info;
}

void FormatConverter::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);
    QString errorMessage;

    if (!success) {
        QByteArray errorOutput = m_ffmpegProcess->readAllStandardError();
        errorMessage = QString::fromUtf8(errorOutput);
    }

    emit conversionFinished(success, errorMessage);
}

void FormatConverter::onProcessError(QProcess::ProcessError error)
{
    QString errorMessage;
    switch (error) {
    case QProcess::FailedToStart:
        errorMessage = "FFmpeg failed to start. Make sure FFmpeg is installed.";
        break;
    case QProcess::Crashed:
        errorMessage = "FFmpeg crashed during conversion.";
        break;
    case QProcess::Timedout:
        errorMessage = "FFmpeg conversion timed out.";
        break;
    case QProcess::WriteError:
        errorMessage = "Write error during FFmpeg conversion.";
        break;
    case QProcess::ReadError:
        errorMessage = "Read error during FFmpeg conversion.";
        break;
    default:
        errorMessage = "Unknown FFmpeg error.";
    }

    emit conversionError(errorMessage);
}

QString FormatConverter::buildFfmpegCommand(const QString &inputPath, const QString &outputPath, const QString &format, const QVariantMap &options)
{
    QStringList args;

    // Input file
    args << "-i" << inputPath;

    // Audio only conversion
    if (options.value("audio_only", false).toBool()) {
        args << "-vn"; // No video
        if (options.contains("audio_codec")) {
            args << "-c:a" << options["audio_codec"].toString();
        } else {
            args << "-c:a" << "libmp3lame"; // Default to MP3
        }
    } else {
        // Video conversion
        args << "-c:v" << "libx264"; // Default video codec
        args << "-c:a" << "aac"; // Default audio codec
    }

    // Quality/bitrate options
    if (options.contains("bitrate")) {
        args << "-b:v" << options["bitrate"].toString();
    }

    // Resolution
    if (options.contains("resolution")) {
        args << "-s" << options["resolution"].toString();
    }

    // Output format
    if (!format.isEmpty()) {
        args << "-f" << format;
    }

    // Overwrite output
    args << "-y";

    // Output file
    args << outputPath;

    return args.join(' ');
}

void FormatConverter::parseProgress(const QString &output)
{
    // Parse FFmpeg progress output to emit conversionProgress signal
    // This is a simplified implementation
    QRegularExpression regex("time=(\\d{2}):(\\d{2}):(\\d{2})\\.\\d{2}");
    QRegularExpressionMatch match = regex.match(output);
    if (match.hasMatch()) {
        // Calculate progress based on time - would need total duration
        // For now, emit dummy progress
        emit conversionProgress(50);
    }
}