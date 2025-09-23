#ifndef DOWNLOADDETAILSPANEL_H
#define DOWNLOADDETAILSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QFrame>
#include <QTabWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QSplitter>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QDateTime>
#include <QFileInfo>
#include <QUrl>
#include <QPixmap>
#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QSettings>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>
#include <QSpacerItem>
#include "SpeedChart.h"

// Forward declarations
class SegmentViewer;
class FilePreviewWidget;

/**
 * Download details panel that reproduces IDM's bottom panel exactly
 * Features:
 * - Real-time speed chart (green line like IDM)
 * - Download segments visualization with colored bars
 * - Complete file information (name, URL, size, type)
 * - Download statistics (average speed, elapsed time, ETA)
 * - Action buttons (Resume, Pause, Restart, Open Folder)
 * - Tabbed interface for General/Advanced/Log information
 * - IDM-style visual appearance and layout
 */
class DownloadDetailsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadDetailsPanel(QWidget *parent = nullptr);
    ~DownloadDetailsPanel();

    // === PUBLIC INTERFACE ===
    
    // Download information
    void setDownloadInfo(int downloadId, const QString &filename, const QString &url,
                        qint64 totalSize, const QString &mimeType = QString());
    void updateDownloadProgress(qint64 bytesReceived, qint64 totalSize, int speed,
                               const QString &status, int timeLeft = -1);
    void updateSegments(const QList<QPair<qint64, qint64>> &segments, 
                       const QList<bool> &segmentStatus);
    void clearDownloadInfo();
    
    // Speed chart
    void addSpeedDataPoint(int speed, const QDateTime &timestamp = QDateTime::currentDateTime());
    void resetSpeedChart();
    void setSpeedChartVisible(bool visible);
    
    // UI state
    void setEnabled(bool enabled);
    void saveSettings();
    void loadSettings();
    void resetToDefaults();

    // === DOWNLOAD STATUS ENUM ===
    enum DownloadStatus {
        StatusIdle,
        StatusConnecting,
        StatusDownloading,
        StatusPaused,
        StatusCompleted,
        StatusError,
        StatusStopped
    };

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    // Action buttons
    void resumeRequested(int downloadId);
    void pauseRequested(int downloadId);
    void stopRequested(int downloadId);
    void restartRequested(int downloadId);
    void openFileRequested(int downloadId);
    void openFolderRequested(int downloadId);
    void propertiesRequested(int downloadId);
    
    // Advanced actions
    void scheduleRequested(int downloadId);
    void priorityChangeRequested(int downloadId, int priority);

private slots:
    void onResumeClicked();
    void onPauseClicked();
    void onStopClicked();
    void onRestartClicked();
    void onOpenFileClicked();
    void onOpenFolderClicked();
    void onPropertiesClicked();
    void onScheduleClicked();
    void onUpdateTimer();
    void onTabChanged(int index);

private:
    // === DATA STRUCTURES ===
    
    struct DownloadInfo {
        int id;
        QString filename;
        QString fullPath;
        QString url;
        QString mimeType;
        qint64 totalSize;
        qint64 bytesReceived;
        int currentSpeed;
        int averageSpeed;
        DownloadStatus status;
        QDateTime startTime;
        QDateTime lastUpdateTime;
        int timeLeft;
        QString errorMessage;
        
        DownloadInfo() : id(-1), totalSize(0), bytesReceived(0), currentSpeed(0),
                        averageSpeed(0), status(StatusIdle), timeLeft(-1) {}
    };

    struct SpeedData {
        QDateTime timestamp;
        int speed;
        
        SpeedData(int s, const QDateTime &t = QDateTime::currentDateTime()) 
            : timestamp(t), speed(s) {}
    };

    // === MEMBER VARIABLES ===
    
    // Main layout
    QVBoxLayout *m_mainLayout;
    QSplitter *m_mainSplitter;
    
    // Top section - File info and actions
    QGroupBox *m_infoGroup;
    QGridLayout *m_infoLayout;
    QLabel *m_filenameLabel;
    QLabel *m_filenameValue;
    QLabel *m_urlLabel;
    QLabel *m_urlValue;
    QLabel *m_sizeLabel;
    QLabel *m_sizeValue;
    QLabel *m_typeLabel;
    QLabel *m_typeValue;
    QLabel *m_statusLabel;
    QLabel *m_statusValue;
    
    // Action buttons (IDM style)
    QHBoxLayout *m_buttonsLayout;
    QPushButton *m_resumeButton;
    QPushButton *m_pauseButton;
    QPushButton *m_stopButton;
    QPushButton *m_restartButton;
    QPushButton *m_openFileButton;
    QPushButton *m_openFolderButton;
    QPushButton *m_propertiesButton;
    QPushButton *m_scheduleButton;
    
    // Progress section
    QGroupBox *m_progressGroup;
    QVBoxLayout *m_progressLayout;
    QProgressBar *m_mainProgressBar;
    QLabel *m_progressText;
    
    // Statistics section
    QGroupBox *m_statsGroup;
    QGridLayout *m_statsLayout;
    QLabel *m_speedLabel;
    QLabel *m_speedValue;
    QLabel *m_avgSpeedLabel;
    QLabel *m_avgSpeedValue;
    QLabel *m_elapsedLabel;
    QLabel *m_elapsedValue;
    QLabel *m_etaLabel;
    QLabel *m_etaValue;
    QLabel *m_segmentsLabel;
    QLabel *m_segmentsValue;
    
    // Bottom section - Tabbed interface
    QTabWidget *m_tabWidget;
    
    // Speed Chart tab
    QWidget *m_chartTab;
    QVBoxLayout *m_chartLayout;
    SpeedChart *m_speedChart;
    QLabel *m_chartStatsLabel;
    
    // Segments tab
    QWidget *m_segmentsTab;
    QVBoxLayout *m_segmentsLayout;
    SegmentViewer *m_segmentViewer;
    QTableWidget *m_segmentTable;
    
    // General tab
    QWidget *m_generalTab;
    QVBoxLayout *m_generalLayout;
    QLabel *m_downloadPathLabel;
    QLabel *m_downloadPathValue;
    QLabel *m_addedTimeLabel;
    QLabel *m_addedTimeValue;
    QLabel *m_lastModifiedLabel;
    QLabel *m_lastModifiedValue;
    FilePreviewWidget *m_previewWidget;
    
    // Log tab
    QWidget *m_logTab;
    QVBoxLayout *m_logLayout;
    QTextEdit *m_logTextEdit;
    QPushButton *m_clearLogButton;
    QPushButton *m_saveLogButton;
    
    // Data
    DownloadInfo m_currentDownload;
    QList<SpeedData> m_speedHistory;
    QList<QPair<qint64, qint64>> m_segments;
    QList<bool> m_segmentStatus;
    QStringList m_logMessages;
    
    // UI state
    QSettings *m_settings;
    QTimer *m_updateTimer;
    bool m_panelEnabled;
    
    // Visual styling
    QFont m_labelFont;
    QFont m_valueFont;
    QFont m_titleFont;
    QColor m_backgroundColor;
    QColor m_borderColor;
    QColor m_textColor;
    QColor m_accentColor;
    
    // Animation
    QPropertyAnimation *m_progressAnimation;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    // === SETUP METHODS ===
    void setupUI();
    void setupInfoSection();
    void setupActionButtons();
    void setupProgressSection();
    void setupStatsSection();
    void setupTabbedInterface();
    void setupSpeedChartTab();
    void setupSegmentsTab();
    void setupGeneralTab();
    void setupLogTab();
    void applyIDMStyle();
    void setupAnimations();
    
    // === UI UPDATE METHODS ===
    void updateFileInfo();
    void updateProgressInfo();
    void updateStatistics();
    void updateActionButtons();
    void updateSpeedChart();
    void updateSegmentViewer();
    void updateLogDisplay();
    void refreshAllSections();
    
    // === UTILITY METHODS ===
    QString formatFileSize(qint64 bytes) const;
    QString formatSpeed(int bytesPerSecond) const;
    QString formatTime(int seconds) const;
    QString formatElapsedTime(const QDateTime &startTime) const;
    QString formatETA(int secondsLeft) const;
    QString getStatusText(DownloadStatus status) const;
    QColor getStatusColor(DownloadStatus status) const;
    QIcon getFileTypeIcon(const QString &filename) const;
    QString getMimeTypeDescription(const QString &mimeType) const;
    
    // === DATA MANAGEMENT ===
    void addLogMessage(const QString &message, const QString &type = "INFO");
    void updateSpeedStatistics();
    void calculateAverageSpeed();
    void cleanupOldSpeedData();
    void updateSegmentTable();
    
    // === VISUAL STYLING ===
    void styleButton(QPushButton *button, const QString &iconName, bool primary = false);
    void styleLabel(QLabel *label, bool isValue = false);
    void styleGroupBox(QGroupBox *groupBox);
    void styleProgressBar(QProgressBar *progressBar);
    void updateProgressBarStyle(int percentage);
    
    // === ANIMATION UTILITIES ===
    void animateProgressChange(int newProgress);
    void highlightSpeedIncrease();
    void showCompletionEffect();
    
    // === CONSTANTS ===
    static const int UPDATE_INTERVAL = 1000;           // Update every second
    static const int SPEED_HISTORY_SIZE = 300;         // Keep 5 minutes of data
    static const int CHART_HEIGHT = 120;               // Speed chart height
    static const int SEGMENT_HEIGHT = 20;              // Segment bar height
    static const int BUTTON_HEIGHT = 24;               // Action button height
    static const int BUTTON_WIDTH = 80;                // Action button width
    static const int PROGRESS_HEIGHT = 16;             // Progress bar height
    static const int PANEL_MIN_HEIGHT = 200;           // Minimum panel height
    static const int PANEL_DEFAULT_HEIGHT = 250;       // Default panel height
    static const int INFO_SECTION_HEIGHT = 120;        // Info section height
    static const int STATS_SECTION_HEIGHT = 80;        // Stats section height
    static const int TAB_SECTION_MIN_HEIGHT = 150;     // Tab section minimum height
};

/**
 * Download segments visualization widget
 */
class SegmentViewer : public QWidget
{
    Q_OBJECT

public:
    explicit SegmentViewer(QWidget *parent = nullptr);
    
    void setSegments(const QList<QPair<qint64, qint64>> &segments, 
                    const QList<bool> &status);
    void setTotalSize(qint64 totalSize);
    void updateProgress(const QList<qint64> &progress);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    struct Segment {
        qint64 start;
        qint64 end;
        qint64 progress;
        bool active;
    };
    
    QList<Segment> m_segments;
    qint64 m_totalSize;
    QPoint m_mousePos;
    
    void drawSegmentBars(QPainter &painter);
    void drawSegmentTooltip(QPainter &painter);
    QColor getSegmentColor(const Segment &segment) const;
    QString formatSegmentTooltip(const Segment &segment) const;
};

/**
 * File preview widget for supported file types
 */
class FilePreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilePreviewWidget(QWidget *parent = nullptr);
    
    void setFile(const QString &filepath);
    void clearPreview();
    bool canPreview(const QString &filepath) const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_filepath;
    QPixmap m_preview;
    QLabel *m_previewLabel;
    QLabel *m_noPreviewLabel;
    
    void generatePreview();
    QPixmap createImagePreview(const QString &filepath);
    QPixmap createTextPreview(const QString &filepath);
    QPixmap createVideoPreview(const QString &filepath);
};

#endif // DOWNLOADDETAILSPANEL_H