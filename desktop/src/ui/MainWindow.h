#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QProgressBar>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QTabWidget>
#include <QTextEdit>
#include <QSpinBox>
#include <QSettings>
#include <QTimer>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>

// Forward declarations for custom widgets
class DownloadTableWidget;
class CategorySidebar;
class DownloadDetailsPanel;
class CustomProgressBar;
class SpeedChart;
class AddUrlDialog;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Public interface for external interactions
    void addDownloadUrl(const QString &url, const QString &filename = QString());
    void showAddUrlDialog(const QString &url = QString());
    void showSettingsDialog();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    // File Menu Actions
    void onNewDownload();
    void onAddBatch();
    void onImportFromBrowser();
    void onExportDownloads();
    void onExit();

    // Edit Menu Actions
    void onSelectAll();
    void onSelectCompleted();
    void onSelectUncompleted();
    void onInvertSelection();
    void onCopyUrlToClipboard();
    void onProperties();

    // Downloads Menu Actions
    void onResumeSelected();
    void onResumeAll();
    void onPauseSelected();
    void onPauseAll();
    void onStopSelected();
    void onStopAll();
    void onRemoveSelected();
    void onRemoveCompleted();
    void onRemoveAll();
    void onMoveToTop();
    void onMoveUp();
    void onMoveDown();
    void onMoveToBottom();

    // View Menu Actions
    void onToolbarToggle();
    void onStatusBarToggle();
    void onCategoriesToggle();
    void onDetailsToggle();
    void onRefreshView();
    void onCustomizeColumns();

    // Tools Menu Actions
    void onOptions();
    void onScheduler();
    void onSpeedLimiter();
    void onSiteExplorer();
    void onVideoDownloader();
    void onBrowserIntegration();

    // Help Menu Actions
    void onHelp();
    void onKeyboardShortcuts();
    void onCheckUpdates();
    void onAbout();

    // Toolbar Actions
    void onToolbarAddUrl();
    void onToolbarStartAll();
    void onToolbarPauseAll();
    void onToolbarStopAll();
    void onToolbarRemove();
    void onToolbarSettings();

    // Category Selection
    void onCategoryChanged(const QString &category);
    void onCategoryContextMenu(const QPoint &pos);

    // Download Table Events
    void onDownloadSelectionChanged();
    void onDownloadDoubleClicked(int row);
    void onDownloadContextMenu(const QPoint &pos);
    void onDownloadItemChanged(QTableWidgetItem *item);

    // Download Engine Events
    void onDownloadStarted(int downloadId);
    void onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal, int speed);
    void onDownloadPaused(int downloadId);
    void onDownloadResumed(int downloadId);
    void onDownloadCompleted(int downloadId);
    void onDownloadFailed(int downloadId, const QString &error);
    void onDownloadRemoved(int downloadId);

    // System Tray
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onTrayIconContextMenu();

    // UI Updates
    void onUpdateTimer();
    void onThemeChanged();
    void onSettingsChanged();

private:
    // === CORE UI COMPONENTS (IDM Layout) ===
    
    // Main layout structure
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QSplitter *m_horizontalSplitter;
    QSplitter *m_verticalSplitter;

    // Menu bar (exact IDM structure)
    QMenuBar *m_menuBar;
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_downloadsMenu;
    QMenu *m_viewMenu;
    QMenu *m_toolsMenu;
    QMenu *m_helpMenu;

    // Toolbar (IDM style)
    QToolBar *m_mainToolBar;
    QAction *m_addUrlAction;
    QAction *m_startAllAction;
    QAction *m_pauseAllAction;
    QAction *m_stopAllAction;
    QAction *m_removeAction;
    QAction *m_settingsAction;

    // Left sidebar - Categories (IDM style)
    CategorySidebar *m_categorySidebar;

    // Center area - Downloads table (IDM exact layout)
    DownloadTableWidget *m_downloadTable;

    // Bottom panel - Download details (IDM style)
    DownloadDetailsPanel *m_detailsPanel;

    // Status bar (IDM style)
    QStatusBar *m_statusBar;
    QLabel *m_statusDownloads;
    QLabel *m_statusSpeed;
    QLabel *m_statusRemaining;

    // === DIALOGS ===
    AddUrlDialog *m_addUrlDialog;
    SettingsDialog *m_settingsDialog;

    // === SYSTEM INTEGRATION ===
    QSystemTrayIcon *m_systemTray;
    QMenu *m_trayMenu;

    // === DATA MANAGEMENT ===
    QSettings *m_settings;
    QTimer *m_updateTimer;

    // Current state
    QString m_currentCategory;
    QList<int> m_selectedDownloads;
    int m_currentDownloadId;

    // === SETUP METHODS ===
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupSystemTray();
    void setupSplitters();
    void setupConnections();
    void setupShortcuts();
    void setupDragDrop();

    // === IDM STYLE APPLICATION ===
    void applyIDMStyle();
    void loadIDMStyleSheet();
    void setupIDMIcons();
    void configureIDMLayout();

    // === LAYOUT MANAGEMENT ===
    void saveLayoutSettings();
    void loadLayoutSettings();
    void resetLayoutToDefault();
    void updateLayoutForResponsive();

    // === DOWNLOAD MANAGEMENT ===
    void refreshDownloadList();
    void updateDownloadCounts();
    void filterDownloadsByCategory();
    void sortDownloadsByColumn(int column, Qt::SortOrder order);

    // === UI STATE MANAGEMENT ===
    void updateUIState();
    void updateToolbarState();
    void updateMenuState();
    void updateStatusBar();
    void updateTrayIcon();

    // === UTILITY METHODS ===
    QString formatFileSize(qint64 bytes) const;
    QString formatSpeed(qint64 bytesPerSecond) const;
    QString formatTime(int seconds) const;
    QString formatETA(int seconds) const;
    QIcon getFileTypeIcon(const QString &filename) const;
    QString getFileCategory(const QString &filename) const;

    // === CONTEXT MENUS ===
    void createDownloadContextMenu();
    void createCategoryContextMenu();
    void createTrayContextMenu();

    // === KEYBOARD SHORTCUTS ===
    void setupIDMShortcuts();

    // === NOTIFICATIONS ===
    void showNotification(const QString &title, const QString &message);
    void showDownloadCompleteNotification(const QString &filename);

    // === CONSTANTS (IDM Values) ===
    static const int DEFAULT_WINDOW_WIDTH = 800;
    static const int DEFAULT_WINDOW_HEIGHT = 600;
    static const int DEFAULT_SIDEBAR_WIDTH = 200;
    static const int DEFAULT_DETAILS_HEIGHT = 150;
    static const int UPDATE_INTERVAL_MS = 1000;
    static const int MAX_RECENT_URLS = 10;

    // === ENUMS ===
    enum DownloadStatus {
        StatusDownloading,
        StatusPaused,
        StatusCompleted,
        StatusError,
        StatusQueued,
        StatusStopped
    };

    enum CategoryType {
        CategoryAll,
        CategoryDownloading,
        CategoryCompleted,
        CategoryUnfinished,
        CategoryVideo,
        CategoryAudio,
        CategoryDocuments,
        CategoryCompressed,
        CategoryPrograms,
        CategoryCustom
    };

    enum ColumnIndex {
        ColumnIcon = 0,
        ColumnFilename,
        ColumnSize,
        ColumnProgress,
        ColumnSpeed,
        ColumnStatus,
        ColumnTimeLeft,
        ColumnAdded,
        ColumnCount
    };
};

#endif // MAINWINDOW_H