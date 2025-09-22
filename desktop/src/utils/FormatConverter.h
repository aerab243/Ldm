#ifndef FORMATCONVERTER_H
#define FORMATCONVERTER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QVariantMap>

class FormatConverter : public QObject
{
    Q_OBJECT

public:
    explicit FormatConverter(QObject *parent = nullptr);
    ~FormatConverter();

    // Conversion methods
    bool convertVideo(const QString &inputPath, const QString &outputPath, const QString &format, const QVariantMap &options = QVariantMap());
    bool convertAudio(const QString &inputPath, const QString &outputPath, const QString &format, const QVariantMap &options = QVariantMap());
    bool extractAudio(const QString &inputPath, const QString &outputPath, const QString &format = "mp3");

    // Utility methods
    QStringList supportedVideoFormats() const;
    QStringList supportedAudioFormats() const;
    QVariantMap getMediaInfo(const QString &inputPath);

signals:
    void conversionProgress(int percentage);
    void conversionFinished(bool success, const QString &errorMessage);
    void conversionError(const QString &error);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    QProcess *m_ffmpegProcess;
    QString m_currentInput;
    QString m_currentOutput;

    QString buildFfmpegCommand(const QString &inputPath, const QString &outputPath, const QString &format, const QVariantMap &options);
    void parseProgress(const QString &output);
};

#endif // FORMATCONVERTER_H