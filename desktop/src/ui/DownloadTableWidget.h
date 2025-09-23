#ifndef DOWNLOADTABLEWIDGET_H
#define DOWNLOADTABLEWIDGET_H

#include <QTableWidget>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QProgressBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QItemDelegate>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDrag>
#include <QFileInfo>
#include <QDateTime>
#include <QSettings>

// Forward declarations
class CustomProgressBar;
class FileTypeIconProvider;

/**
 * Custom table widget that reproduces IDM's download table exactly
 * Features:
 * - IDM-style columns layout
 * - Custom progress bars with embedded percentages
 * - File type icons
 * - Speed formatting with colors
 * - Status indicators with colors
 * - Context menu identical to IDM
 * - Drag & drop support
 * - Keyboard shortcuts
 * - Multi-selection with checkboxes
 * - Column sorting with visual indicators
 */
class DownloadTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit DownloadTableWidget(QWidget *parent = nullptr);
    ~DownloadTableWidget();

    // === PUBLIC INTERFACE ===
    
    // Download management
    int addDownload(const QString &url, const QString &filename, qint64 totalSize = 0);
    void updateDownload(int downloadId, qint64 bytesReceived, qint64 totalSize, 
                       int speed, const QString &status, int timeLeft = -1);
    void removeDownload(int downloadId);
    void removeAllDownloads();
    void removeCompletedDownloads();
    
    // Selection management
    QList<int> getSelectedDownloadIds() const;
    void selectAll();
    void selectCompleted();
    void selectUncompleted();
    void invertSelection();
    void clearSelection();
    
    // Filtering and sorting
    void filterByCategory(const QString &category);
    void filterByStatus(const QString &status);
    void sortByColumn(int column, Qt::SortOrder order = Qt::AscendingOrder);
    
    // UI state
    void setColumnVisibility(int column, bool visible);
    bool isColumnVisible(int column) const;
    void resetColumnsToDefault();
    void saveColumnSettings();
    void loadColumnSettings();
    
    // Data access
    int getDownloadCount() const;
    int getDownloadCount(const QString &status) const;
    qint64 getTotalSize() const;
    qint64 getTotalDownloaded() const;
    int getTotalSpeed() const;

    // === IDM COLUMN INDICES ===
    enum ColumnIndex {
        ColumnIcon = 0,      // File type icon
        ColumnFilename,      // Filename with path
        ColumnSize,          // File size (formatted)
        ColumnProgress,      // Progress bar with percentage
        ColumnSpeed,         // Download speed (formatted with colors)
        ColumnStatus,        // Status with colors
        ColumnTimeLeft,      // Estimated time remaining
        ColumnAdded,         // Date/time added
        ColumnCount
    };

    // === DOWNLOAD STATUS ENUM ===
    enum DownloadStatus {
        StatusDownloading,
        StatusPaused,
        StatusCompleted,
        StatusError,
        StatusQueued,
        StatusStopped,
        StatusConnecting
    };

protected:
    // Event handlers
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
    // Drag & drop
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

signals:
    // Download actions
    void downloadResumeRequested(int downloadId);
    void downloadPauseRequested(int downloadId);
    void downloadStopRequested(int downloadId);
    void downloadRemoveRequested(int downloadId);
    void downloadRestartRequested(int downloadId);
    void downloadPropertiesRequested(int downloadId);
    
    // File operations
    void openFileRequested(int downloadId);
    void openFolderRequested(int downloadId);
    void copyUrlRequested(int downloadId);
    
    // Selection events
    void selectionChanged(const QList<int> &selectedIds);
    void downloadDoubleClicked(int downloadId);
    
    // UI events
    void contextMenuRequested(const QPoint &pos, int downloadId);

private slots:
    void onItemSelectionChanged();
    void onHeaderSectionClicked(int logicalIndex);
    void showContextMenu(const QPoint &pos);
    void onItemDoubleClicked(QTableWidgetItem *item);
    void onHeaderSectionResized(int logicalIndex, int oldSize, int newSize);
    void onUpdateTimer();
    void onAnimationFinished();
    
    // Context menu actions
    void onResumeSelected();
    void onPauseSelected();
    void onStopSelected();
    void onRemoveSelected();
    void onRestartSelected();
    void onOpenFile();
    void onOpenFolder();
    void onCopyUrl();
    void onProperties();
    void onMoveToTop();
    void onMoveUp();
    void onMoveDown();
    void onMoveToBottom();

private:
    // === DATA STRUCTURES ===
    
    struct DownloadItem {
        int id;
        QString url;
        QString filename;
        QString fullPath;
        qint64 totalSize;
        qint64 bytesReceived;
        int speed;
        DownloadStatus status;
        QString category;
        QDateTime addedTime;
        QDateTime completedTime;
        int timeLeft;
        QString errorMessage;
        
        DownloadItem() : id(-1), totalSize(0), bytesReceived(0), speed(0), 
                        status(StatusQueued), timeLeft(-1) {}
    };

    // === MEMBER VARIABLES ===
    
    // Data storage
    QList<DownloadItem> m_downloads;
    QMap<int, int> m_downloadIdToRow;
    int m_nextDownloadId;
    
    // UI components
    QMenu *m_contextMenu;
    QTimer *m_updateTimer;
    QSettings *m_settings;
    
    // Context menu actions
    QAction *m_resumeAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
    QAction *m_removeAction;
    QAction *m_restartAction;
    QAction *m_openFileAction;
    QAction *m_openFolderAction;
    QAction *m_copyUrlAction;
    QAction *m_propertiesAction;
    QAction *m_moveToTopAction;
    QAction *m_moveUpAction;
    QAction *m_moveDownAction;
    QAction *m_moveToBottomAction;
    
    // Visual state
    QString m_currentFilter;
    QString m_currentCategory;
    int m_sortColumn;
    Qt::SortOrder m_sortOrder;
    QMap<int, bool> m_columnVisibility;
    
    // Animation support
    QPropertyAnimation *m_progressAnimation;
    QList<int> m_animatingRows;
    
    // === SETUP METHODS ===
    void setupTable();
    void setupContextMenu();
    void setupHeader();
    void setupDragDrop();
    void setupAnimations();
    void applyIDMStyle();
    
    // === TABLE MANAGEMENT ===
    void insertDownloadRow(const DownloadItem &download);
    void updateDownloadRow(int row, const DownloadItem &download);
    void removeDownloadRow(int row);
    void refreshTable();
    void updateRowColors();
    void updateProgressBars();
    
    // === DATA UTILITIES ===
    int findDownloadById(int downloadId) const;
    int findRowByDownloadId(int downloadId) const;
    DownloadItem* getDownloadByRow(int row);
    const DownloadItem* getDownloadByRow(int row) const;
    bool matchesFilter(const DownloadItem &download) const;
    
    // === FORMATTING UTILITIES ===
    QString formatFileSize(qint64 bytes) const;
    QString formatSpeed(int bytesPerSecond) const;
    QString formatTimeRemaining(int seconds) const;
    QString formatDateTime(const QDateTime &dateTime) const;
    QString getStatusText(DownloadStatus status) const;
    QColor getStatusColor(DownloadStatus status) const;
    QColor getSpeedColor(int speed) const;
    
    // === ICON MANAGEMENT ===
    QIcon getFileTypeIcon(const QString &filename) const;
    QIcon getStatusIcon(DownloadStatus status) const;
    
    // === SELECTION UTILITIES ===
    void updateSelectionActions();
    bool hasSelectedDownloads() const;
    int getSelectedDownloadCount() const;
    
    // === ANIMATION UTILITIES ===
    void animateProgressBar(int row);
    void highlightNewDownload(int row);
    void fadeOutRemovedDownload(int row);
    
    // === COLUMN MANAGEMENT ===
    void setupColumnHeaders();
    void setupColumnWidths();
    void setupColumnAlignment();
    void setupColumnSorting();
    
    // === KEYBOARD SHORTCUTS ===
    void setupKeyboardShortcuts();
    void handleKeyPress(QKeyEvent *event);
    
    // === DRAG & DROP UTILITIES ===
    void handleUrlDrop(const QStringList &urls);
    QMimeData* createDragData(const QList<int> &downloadIds);
    
    // === CONSTANTS ===
    static const int UPDATE_INTERVAL = 1000;           // Update every second
    static const int ANIMATION_DURATION = 300;         // Animation duration in ms
    static const int DEFAULT_ROW_HEIGHT = 20;          // Default row height
    static const int PROGRESS_BAR_HEIGHT = 16;         // Progress bar height
    static const int ICON_SIZE = 16;                   // Icon size
    static const int MIN_COLUMN_WIDTH = 50;            // Minimum column width
    static const int DEFAULT_FILENAME_WIDTH = 250;     // Default filename column width
    static const int DEFAULT_SIZE_WIDTH = 80;          // Default size column width
    static const int DEFAULT_PROGRESS_WIDTH = 100;     // Default progress column width
    static const int DEFAULT_SPEED_WIDTH = 80;         // Default speed column width
    static const int DEFAULT_STATUS_WIDTH = 100;       // Default status column width
    static const int DEFAULT_TIME_WIDTH = 80;          // Default time column width
    static const int DEFAULT_ADDED_WIDTH = 120;        // Default added column width
};

/**
 * Custom item delegate for IDM-style rendering
 */
class DownloadItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit DownloadItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const override;

private:
    void paintProgressBar(QPainter *painter, const QStyleOptionViewItem &option,
                         int progress, const QString &text) const;
    void paintSpeedText(QPainter *painter, const QStyleOptionViewItem &option,
                       const QString &speed, const QColor &color) const;
    void paintStatusText(QPainter *painter, const QStyleOptionViewItem &option,
                        const QString &status, const QColor &color) const;
    void paintFileIcon(QPainter *painter, const QStyleOptionViewItem &option,
                      const QIcon &icon) const;
};

#endif // DOWNLOADTABLEWIDGET_H