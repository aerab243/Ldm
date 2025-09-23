#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTextEdit>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QFileInfo>
#include <QDir>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QSettings>
#include <QSizePolicy>
#include <QPixmap>
#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QRegularExpression>
#include <QValidator>
#include <QCompleter>
#include <QStringListModel>
#include <QStandardPaths>
#include <QDateTime>

// Forward declarations
class Database;
class FileTypeDetector;
class UrlValidator;
class CategoryManager;

/**
 * Add URL dialog that reproduces IDM's "Add Download" dialog exactly
 * Features:
 * - URL input with real-time validation and preview
 * - Automatic filename detection and suggestion
 * - Category selection with custom categories
 * - Save path selection with history
 * - Tabbed interface: General, Connection, Advanced
 * - Connection settings: segments, speed limit, proxy
 * - Advanced options: user agent, referer, cookies, headers
 * - File preview and type detection
 * - Download later/scheduler options
 * - IDM-style visual appearance and layout
 */
class AddUrlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUrlDialog(Database *database, QWidget *parent = nullptr);
    explicit AddUrlDialog(const QString &url, QWidget *parent = nullptr);
    ~AddUrlDialog();

    // === PUBLIC INTERFACE ===
    
    // URL and file information
    void setUrl(const QString &url);
    QString getUrl() const;
    void setFilename(const QString &filename);
    QString getFileName() const;
    void setSavePath(const QString &path);
    QString getSavePath() const;
    QString getFullFilePath() const;
    
    // Category and options
    void setCategory(const QString &category);
    QString getCategory() const;
    int getCategoryId() const;
    void setStartDownload(bool start);
    bool shouldStartDownload() const;
    void setScheduleDownload(bool schedule);
    bool shouldScheduleDownload() const;
    
    // Connection settings
    void setSegmentCount(int count);
    int getSegmentCount() const;
    void setSpeedLimit(int limitKBps);
    int getSpeedLimit() const;
    void setProxySettings(const QString &proxy, int port, const QString &username = QString(), const QString &password = QString());
    
    // Advanced settings
    void setUserAgent(const QString &userAgent);
    QString getUserAgent() const;
    void setReferer(const QString &referer);
    QString getReferer() const;
    void setCookies(const QString &cookies);
    QString getCookies() const;
    void setHeaders(const QStringList &headers);
    QStringList getHeaders() const;
    
    // Dialog state
    void resetToDefaults();
    void loadFromClipboard();
    void saveSettings();
    void loadSettings();

    // === RESULT STRUCTURE ===
    struct DownloadOptions {
        QString url;
        QString filename;
        QString savePath;
        QString category;
        bool startImmediately;
        bool scheduleDownload;
        QDateTime scheduledTime;
        
        // Connection settings
        int segments;
        int speedLimitKBps;
        bool useProxy;
        QString proxyHost;
        int proxyPort;
        QString proxyUsername;
        QString proxyPassword;
        
        // Advanced settings
        QString userAgent;
        QString referer;
        QString cookies;
        QStringList customHeaders;
        bool resumeCapable;
        int timeout;
        int retryCount;
        
        DownloadOptions() : startImmediately(true), scheduleDownload(false),
                           segments(8), speedLimitKBps(0), useProxy(false),
                           proxyPort(8080), resumeCapable(true), timeout(30), retryCount(3) {}
    };
    
    DownloadOptions getDownloadOptions() const;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void downloadRequested(const DownloadOptions &options);
    void urlValidated(bool isValid, const QString &filename, qint64 fileSize);
    void fileInfoDetected(const QString &mimeType, const QString &description);

private slots:
    // URL handling
    void onUrlChanged();
    void onUrlTextChanged(const QString &text);
    void onPasteUrlClicked();
    void onClearUrlClicked();
    void onValidateUrlClicked();
    
    // File handling
    void onFilenameChanged();
    void onBrowseSavePathClicked();
    void onOpenFolderClicked();
    void onCategoryChanged();
    void onBrowseClicked();
    
    // Button actions
    void onOkClicked();
    void onCancelClicked();
    void onDownloadLaterClicked();
    void onScheduleClicked();
    void onResetClicked();
    void onAdvancedToggled();
    
    // Tab changes
    void onTabChanged(int index);
    
    // Network operations
    void onUrlValidationFinished();
    void onFileInfoReceived();
    void onNetworkError(QNetworkReply::NetworkError error);
    
    // Auto-completion
    void onUrlCompletionActivated(const QString &text);
    void onPathCompletionActivated(const QString &text);
    
    // Settings changes
    void onSegmentCountChanged(int count);
    void onSpeedLimitChanged(int limit);
    void onProxySettingsChanged();
    void onAdvancedSettingsChanged();

private:
    // === MAIN UI COMPONENTS ===
    
    // Main layout
    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;
    
    // Button bar
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_downloadLaterButton;
    QPushButton *m_scheduleButton;
    QPushButton *m_resetButton;
    QPushButton *m_advancedButton;
    
    // === GENERAL TAB ===
    QWidget *m_generalTab;
    QVBoxLayout *m_generalLayout;
    
    // URL section
    QGroupBox *m_urlGroup;
    QGridLayout *m_urlLayout;
    QLabel *m_urlLabel;
    QLineEdit *m_urlEdit;
    QPushButton *m_pasteUrlButton;
    QPushButton *m_clearUrlButton;
    QPushButton *m_validateUrlButton;
    QLabel *m_urlStatusLabel;
    QProgressBar *m_validationProgress;
    
    // File info section
    QGroupBox *m_fileGroup;
    QGridLayout *m_fileLayout;
    QLabel *m_filenameLabel;
    QLineEdit *m_filenameEdit;
    QLineEdit *m_fileNameEdit;
    QPushButton *m_browseButton;
    QLabel *m_sizeLabel;
    QLabel *m_sizeValue;
    QLabel *m_typeLabel;
    QLabel *m_typeValue;
    QLabel *m_lastModifiedLabel;
    QLabel *m_lastModifiedValue;
    
    // Save location section
    QGroupBox *m_saveGroup;
    QGridLayout *m_saveLayout;
    QLabel *m_savePathLabel;
    QLineEdit *m_savePathEdit;
    QPushButton *m_browseSaveButton;
    QPushButton *m_openFolderButton;
    QLabel *m_categoryLabel;
    QComboBox *m_categoryCombo;
    
    // Options section
    QGroupBox *m_optionsGroup;
    QVBoxLayout *m_optionsLayout;
    QCheckBox *m_startDownloadCheck;
    QCheckBox *m_addToQueueCheck;
    QCheckBox *m_useDefaultPathCheck;
    QCheckBox *m_shutdownWhenCompleteCheck;
    
    // === CONNECTION TAB ===
    QWidget *m_connectionTab;
    QVBoxLayout *m_connectionLayout;
    
    // Connection settings
    QGroupBox *m_connectionGroup;
    QGridLayout *m_connectionGridLayout;
    QLabel *m_segmentsLabel;
    QSpinBox *m_segmentsSpin;
    QLabel *m_speedLimitLabel;
    QSpinBox *m_speedLimitSpin;
    QLabel *m_speedUnitLabel;
    QLabel *m_timeoutLabel;
    QSpinBox *m_timeoutSpin;
    QLabel *m_retryLabel;
    QSpinBox *m_retrySpin;
    
    // Proxy settings
    QGroupBox *m_proxyGroup;
    QGridLayout *m_proxyLayout;
    QCheckBox *m_useProxyCheck;
    QLabel *m_proxyTypeLabel;
    QComboBox *m_proxyTypeCombo;
    QLabel *m_proxyHostLabel;
    QLineEdit *m_proxyHostEdit;
    QLabel *m_proxyPortLabel;
    QSpinBox *m_proxyPortSpin;
    QLabel *m_proxyUserLabel;
    QLineEdit *m_proxyUserEdit;
    QLabel *m_proxyPasswordLabel;
    QLineEdit *m_proxyPasswordEdit;
    QPushButton *m_testProxyButton;
    
    // === ADVANCED TAB ===
    QWidget *m_advancedTab;
    QVBoxLayout *m_advancedLayout;
    
    // Headers section
    QGroupBox *m_headersGroup;
    QGridLayout *m_headersLayout;
    QLabel *m_userAgentLabel;
    QComboBox *m_userAgentCombo;
    QLabel *m_refererLabel;
    QLineEdit *m_refererEdit;
    QLabel *m_cookiesLabel;
    QTextEdit *m_cookiesEdit;
    
    // Custom headers
    QGroupBox *m_customHeadersGroup;
    QVBoxLayout *m_customHeadersLayout;
    QTextEdit *m_customHeadersEdit;
    QHBoxLayout *m_headersButtonLayout;
    QPushButton *m_addHeaderButton;
    QPushButton *m_removeHeaderButton;
    QPushButton *m_clearHeadersButton;
    
    // Advanced options
    QGroupBox *m_advancedOptionsGroup;
    QVBoxLayout *m_advancedOptionsLayout;
    QCheckBox *m_resumeCapableCheck;
    QCheckBox *m_followRedirectsCheck;
    QCheckBox *m_acceptCookiesCheck;
    QCheckBox *m_validateCertificateCheck;
    
    // === DATA MEMBERS ===
    
    // Database
    Database *m_database;
    
    // Network components
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_validationReply;
    QTimer *m_validationTimer;
    
    // Auto-completion
    QCompleter *m_urlCompleter;
    QCompleter *m_pathCompleter;
    QStringListModel *m_urlModel;
    QStringListModel *m_pathModel;
    
    // Validation and detection
    UrlValidator *m_urlValidator;
    FileTypeDetector *m_fileTypeDetector;
    
    // Settings and state
    QSettings *m_settings;
    DownloadOptions m_currentOptions;
    bool m_urlValid;
    bool m_validating;
    qint64 m_detectedFileSize;
    QString m_detectedMimeType;
    QString m_originalUrl;
    
    // UI state
    bool m_advancedMode;
    QSize m_normalSize;
    QSize m_advancedSize;
    
    // === SETUP METHODS ===
    void setupUI();
    void setupGeneralTab();
    void setupConnectionTab();
    void setupAdvancedTab();
    void setupButtonBar();
    void setupValidation();
    void setupAutoCompletion();
    void setupConnections();
    void applyIDMStyle();
    void setupIcons();
    
    // === VALIDATION METHODS ===
    void validateUrl();
    void detectFileInfo();
    void updateValidationStatus(bool isValid, const QString &message = QString());
    void showValidationProgress(bool show);
    bool isValidUrl(const QString &url) const;
    QString extractFilenameFromUrl(const QString &url);
    void loadCategories();
    QString extractFilenameFromHeaders(const QNetworkReply *reply) const;
    QString suggestFilename(const QString &url, const QString &contentType = QString()) const;
    
    // === UI UPDATE METHODS ===
    void updateFileInfo();
    void updateSaveLocation();
    void updateButtonStates();
    void updateTabVisibility();
    void updateConnectionSettings();
    void updateAdvancedSettings();
    void refreshCategoryList();
    
    // === DATA MANAGEMENT ===
    void populateUserAgents();
    void populateCategories();
    void populateRecentUrls();
    void populateRecentPaths();
    void addToUrlHistory(const QString &url);
    void addToPathHistory(const QString &path);
    void saveRecentItems();
    void loadRecentItems();
    
    // === UTILITY METHODS ===
    QString formatFileSize(qint64 bytes) const;
    QString getDefaultSavePath() const;
    QString getFileExtension(const QString &filename) const;
    QString getMimeTypeDescription(const QString &mimeType) const;
    bool isFileExists(const QString &filepath) const;
    QString getUniqueFilename(const QString &filepath) const;
    
    // === STYLING METHODS ===
    void styleButton(QPushButton *button, const QString &iconName, bool primary = false);
    void styleLineEdit(QLineEdit *lineEdit, bool hasError = false);
    void styleGroupBox(QGroupBox *groupBox);
    void styleTabWidget();
    void updateValidationIndicator(bool valid);
    
    // === DIALOG SIZE MANAGEMENT ===
    void resizeToContent();
    void toggleAdvancedMode();
    void animateResize(const QSize &newSize);
    
    // === CONSTANTS ===
    static const int DEFAULT_WIDTH = 480;
    static const int DEFAULT_HEIGHT = 400;
    static const int ADVANCED_HEIGHT = 600;
    static const int VALIDATION_TIMEOUT = 10000;
    static const int MAX_RECENT_ITEMS = 20;
    static const int MIN_SEGMENTS = 1;
    static const int MAX_SEGMENTS = 32;
    static const int DEFAULT_SEGMENTS = 8;
    static const int MAX_SPEED_LIMIT = 99999;
    static const int DEFAULT_TIMEOUT = 30;
    static const int MAX_RETRIES = 10;
    static const int DEFAULT_PROXY_PORT = 8080;
};

/**
 * URL validator class for real-time validation
 */
class UrlValidator : public QValidator
{
    Q_OBJECT

public:
    explicit UrlValidator(QObject *parent = nullptr);
    
    State validate(QString &input, int &pos) const override;
    bool isValidUrl(const QString &url) const;
    QUrl normalizeUrl(const QString &url) const;

private:
    QRegularExpression m_urlRegex;
    QStringList m_supportedSchemes;
};

/**
 * File type detector for automatic categorization
 */
class FileTypeDetector : public QObject
{
    Q_OBJECT

public:
    explicit FileTypeDetector(QObject *parent = nullptr);
    
    QString detectCategory(const QString &filename) const;
    QString detectCategory(const QString &filename, const QString &mimeType) const;
    QIcon getFileTypeIcon(const QString &filename) const;
    QString getFileDescription(const QString &filename, const QString &mimeType = QString()) const;

private:
    QMap<QString, QString> m_extensionCategories;
    QMap<QString, QString> m_mimeTypeCategories;
    QMap<QString, QIcon> m_categoryIcons;
    
    void initializeCategories();
    void initializeIcons();
};

#endif // ADDDIALOG_H