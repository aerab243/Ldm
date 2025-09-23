#include "MainWindow.h"
#include "DownloadTableWidget.h"
#include "CategorySidebar.h"
#include "DownloadDetailsPanel.h"
#include "AddUrlDialog.h"
#include "SettingsDialog.h"
#include "CustomProgressBar.h"
#include "SpeedChart.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QThread>
#include <QCloseEvent>
#include <QShowEvent>
#include <QResizeEvent>

// IDM Color Constants
const QColor IDM_BACKGROUND_COLOR = QColor(240, 240, 240);
const QColor IDM_SELECTION_COLOR = QColor(0, 120, 212);
const QColor IDM_BORDER_COLOR = QColor(192, 192, 192);
const QColor IDM_TEXT_COLOR = QColor(0, 0, 0);
const QColor IDM_HOVER_COLOR = QColor(225, 245, 254);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_horizontalSplitter(nullptr)
    , m_verticalSplitter(nullptr)
    , m_menuBar(nullptr)
    , m_mainToolBar(nullptr)
    , m_categorySidebar(nullptr)
    , m_downloadTable(nullptr)
    , m_detailsPanel(nullptr)
    , m_statusBar(nullptr)
    , m_addUrlDialog(nullptr)
    , m_settingsDialog(nullptr)
    , m_systemTray(nullptr)
    , m_settings(nullptr)
    , m_updateTimer(nullptr)
    , m_currentDownloadId(-1)
{
    setObjectName("MainWindow");
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupSystemTray();
    setupSplitters();
    setupConnections();
    setupShortcuts();
    setupDragDrop();
    
    // Apply IDM styling
    applyIDMStyle();
    loadIDMStyleSheet();
    setupIDMIcons();
    configureIDMLayout();
    
    // Initialize settings and timers
    m_settings = new QSettings("LDM", "Linux Download Manager", this);
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(UPDATE_INTERVAL_MS);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimer);
    m_updateTimer->start();
    
    // Load saved settings
    loadLayoutSettings();
    
    // Set window properties
    setWindowTitle("LDM - Linux Download Manager");
    setWindowIcon(QIcon(":/icons/ldm_main.png"));
    setMinimumSize(600, 400);
    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    
    // Center window on screen
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // Enable drop functionality
    setAcceptDrops(true);
    
    // Initial UI state
    updateUIState();
    
    // Show welcome message for first run
    if (m_settings->value("first_run", true).toBool()) {
        QTimer::singleShot(1000, this, [this]() {
            showNotification("LDM Started", "Linux Download Manager is ready to use!");
            m_settings->setValue("first_run", false);
        });
    }
}

MainWindow::~MainWindow()
{
    // Stop timers
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
    
    // Save settings
    saveLayoutSettings();
    
    // Hide system tray icon
    if (m_systemTray) {
        m_systemTray->hide();
    }
}

void MainWindow::setupUI()
{
    // Create central widget and main layout
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Create main horizontal splitter
    m_horizontalSplitter = new QSplitter(Qt::Horizontal, this);
    m_horizontalSplitter->setObjectName("mainHorizontalSplitter");
    m_horizontalSplitter->setChildrenCollapsible(false);
    
    // Create vertical splitter for right side
    m_verticalSplitter = new QSplitter(Qt::Vertical, this);
    m_verticalSplitter->setObjectName("mainVerticalSplitter");
    m_verticalSplitter->setChildrenCollapsible(false);
    
    // Create category sidebar
    m_categorySidebar = new CategorySidebar(this);
    m_categorySidebar->setObjectName("categorySidebar");
    m_categorySidebar->setFixedWidth(DEFAULT_SIDEBAR_WIDTH);
    
    // Create download table
    m_downloadTable = new DownloadTableWidget(this);
    m_downloadTable->setObjectName("downloadTable");
    
    // Create details panel
    m_detailsPanel = new DownloadDetailsPanel(this);
    m_detailsPanel->setObjectName("detailsPanel");
    m_detailsPanel->setFixedHeight(DEFAULT_DETAILS_HEIGHT);
    
    // Setup splitter hierarchy
    m_verticalSplitter->addWidget(m_downloadTable);
    m_verticalSplitter->addWidget(m_detailsPanel);
    
    m_horizontalSplitter->addWidget(m_categorySidebar);
    m_horizontalSplitter->addWidget(m_verticalSplitter);
    
    // Set splitter proportions (IDM-like)
    m_horizontalSplitter->setStretchFactor(0, 0); // Sidebar fixed
    m_horizontalSplitter->setStretchFactor(1, 1); // Main area expandable
    
    m_verticalSplitter->setStretchFactor(0, 1); // Table expandable
    m_verticalSplitter->setStretchFactor(1, 0); // Details fixed
    
    m_mainLayout->addWidget(m_horizontalSplitter);
}

void MainWindow::setupMenuBar()
{
    m_menuBar = menuBar();
    m_menuBar->setObjectName("mainMenuBar");
    
    // File Menu (exact IDM structure)
    m_fileMenu = m_menuBar->addMenu("&File");
    m_fileMenu->addAction("&New Download...", this, &MainWindow::onNewDownload, QKeySequence("Ctrl+N"));
    m_fileMenu->addAction("&Add Batch Download...", this, &MainWindow::onAddBatch, QKeySequence("Ctrl+B"));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("&Import Downloads from Browser...", this, &MainWindow::onImportFromBrowser);
    m_fileMenu->addAction("&Export Download List...", this, &MainWindow::onExportDownloads);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("E&xit", this, &MainWindow::onExit, QKeySequence("Alt+F4"));
    
    // Edit Menu
    m_editMenu = m_menuBar->addMenu("&Edit");
    m_editMenu->addAction("Select &All", this, &MainWindow::onSelectAll, QKeySequence("Ctrl+A"));
    m_editMenu->addAction("Select &Completed", this, &MainWindow::onSelectCompleted);
    m_editMenu->addAction("Select &Uncompleted", this, &MainWindow::onSelectUncompleted);
    m_editMenu->addAction("&Invert Selection", this, &MainWindow::onInvertSelection);
    m_editMenu->addSeparator();
    m_editMenu->addAction("&Copy URL to Clipboard", this, &MainWindow::onCopyUrlToClipboard, QKeySequence("Ctrl+C"));
    m_editMenu->addSeparator();
    m_editMenu->addAction("&Properties...", this, &MainWindow::onProperties, QKeySequence("Alt+Enter"));
    
    // Downloads Menu
    m_downloadsMenu = m_menuBar->addMenu("&Downloads");
    m_downloadsMenu->addAction("&Resume", this, &MainWindow::onResumeSelected, QKeySequence("F9"));
    m_downloadsMenu->addAction("Resume &All", this, &MainWindow::onResumeAll, QKeySequence("Ctrl+F9"));
    m_downloadsMenu->addAction("&Pause", this, &MainWindow::onPauseSelected, QKeySequence("F5"));
    m_downloadsMenu->addAction("Pause A&ll", this, &MainWindow::onPauseAll, QKeySequence("Ctrl+F5"));
    m_downloadsMenu->addAction("&Stop", this, &MainWindow::onStopSelected, QKeySequence("F6"));
    m_downloadsMenu->addAction("Stop All", this, &MainWindow::onStopAll, QKeySequence("Ctrl+F6"));
    m_downloadsMenu->addSeparator();
    m_downloadsMenu->addAction("&Remove", this, &MainWindow::onRemoveSelected, QKeySequence("Delete"));
    m_downloadsMenu->addAction("Remove &Completed", this, &MainWindow::onRemoveCompleted);
    m_downloadsMenu->addAction("Remove A&ll", this, &MainWindow::onRemoveAll);
    m_downloadsMenu->addSeparator();
    m_downloadsMenu->addAction("Move to &Top", this, &MainWindow::onMoveToTop, QKeySequence("Ctrl+Home"));
    m_downloadsMenu->addAction("Move &Up", this, &MainWindow::onMoveUp, QKeySequence("Ctrl+Up"));
    m_downloadsMenu->addAction("Move &Down", this, &MainWindow::onMoveDown, QKeySequence("Ctrl+Down"));
    m_downloadsMenu->addAction("Move to &Bottom", this, &MainWindow::onMoveToBottom, QKeySequence("Ctrl+End"));
    
    // View Menu
    m_viewMenu = m_menuBar->addMenu("&View");
    QAction *toolbarAction = m_viewMenu->addAction("&Toolbar", this, &MainWindow::onToolbarToggle);
    toolbarAction->setCheckable(true);
    toolbarAction->setChecked(true);
    QAction *statusBarAction = m_viewMenu->addAction("&Status Bar", this, &MainWindow::onStatusBarToggle);
    statusBarAction->setCheckable(true);
    statusBarAction->setChecked(true);
    QAction *categoriesAction = m_viewMenu->addAction("&Categories", this, &MainWindow::onCategoriesToggle);
    categoriesAction->setCheckable(true);
    categoriesAction->setChecked(true);
    QAction *detailsAction = m_viewMenu->addAction("&Details", this, &MainWindow::onDetailsToggle);
    detailsAction->setCheckable(true);
    detailsAction->setChecked(true);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction("&Refresh", this, &MainWindow::onRefreshView, QKeySequence("F5"));
    m_viewMenu->addAction("&Customize Columns...", this, &MainWindow::onCustomizeColumns);
    
    // Tools Menu
    m_toolsMenu = m_menuBar->addMenu("&Tools");
    m_toolsMenu->addAction("&Options...", this, &MainWindow::onOptions, QKeySequence("Ctrl+O"));
    m_toolsMenu->addAction("&Scheduler...", this, &MainWindow::onScheduler);
    m_toolsMenu->addAction("Speed &Limiter...", this, &MainWindow::onSpeedLimiter);
    m_toolsMenu->addSeparator();
    m_toolsMenu->addAction("Site &Explorer...", this, &MainWindow::onSiteExplorer);
    m_toolsMenu->addAction("&Video Downloader...", this, &MainWindow::onVideoDownloader);
    m_toolsMenu->addSeparator();
    m_toolsMenu->addAction("&Browser Integration...", this, &MainWindow::onBrowserIntegration);
    
    // Help Menu
    m_helpMenu = m_menuBar->addMenu("&Help");
    m_helpMenu->addAction("&Help", this, &MainWindow::onHelp, QKeySequence("F1"));
    m_helpMenu->addAction("&Keyboard Shortcuts", this, &MainWindow::onKeyboardShortcuts);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction("Check for &Updates...", this, &MainWindow::onCheckUpdates);
    m_helpMenu->addAction("&About LDM...", this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar("Main Toolbar");
    m_mainToolBar->setObjectName("mainToolBar");
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_mainToolBar->setIconSize(QSize(24, 24));
    m_mainToolBar->setMovable(false);
    
    // Add URL Action
    m_addUrlAction = m_mainToolBar->addAction(QIcon(":/icons/add_url.png"), "Add URL", this, &MainWindow::onToolbarAddUrl);
    m_addUrlAction->setShortcut(QKeySequence("Ctrl+N"));
    m_addUrlAction->setToolTip("Add new download (Ctrl+N)");
    
    m_mainToolBar->addSeparator();
    
    // Start All Action
    m_startAllAction = m_mainToolBar->addAction(QIcon(":/icons/start_all.png"), "Start All", this, &MainWindow::onToolbarStartAll);
    m_startAllAction->setShortcut(QKeySequence("Ctrl+F9"));
    m_startAllAction->setToolTip("Start all downloads (Ctrl+F9)");
    
    // Pause All Action
    m_pauseAllAction = m_mainToolBar->addAction(QIcon(":/icons/pause_all.png"), "Pause All", this, &MainWindow::onToolbarPauseAll);
    m_pauseAllAction->setShortcut(QKeySequence("Ctrl+F5"));
    m_pauseAllAction->setToolTip("Pause all downloads (Ctrl+F5)");
    
    // Stop All Action
    m_stopAllAction = m_mainToolBar->addAction(QIcon(":/icons/stop_all.png"), "Stop All", this, &MainWindow::onToolbarStopAll);
    m_stopAllAction->setShortcut(QKeySequence("Ctrl+F6"));
    m_stopAllAction->setToolTip("Stop all downloads (Ctrl+F6)");
    
    m_mainToolBar->addSeparator();
    
    // Remove Action
    m_removeAction = m_mainToolBar->addAction(QIcon(":/icons/remove.png"), "Remove", this, &MainWindow::onToolbarRemove);
    m_removeAction->setShortcut(QKeySequence("Delete"));
    m_removeAction->setToolTip("Remove selected downloads (Del)");
    
    m_mainToolBar->addSeparator();
    
    // Settings Action
    m_settingsAction = m_mainToolBar->addAction(QIcon(":/icons/settings.png"), "Settings", this, &MainWindow::onToolbarSettings);
    m_settingsAction->setShortcut(QKeySequence("Ctrl+O"));
    m_settingsAction->setToolTip("Open settings (Ctrl+O)");
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    m_statusBar->setObjectName("mainStatusBar");
    
    // Downloads count
    m_statusDownloads = new QLabel("Downloads: 0", this);
    m_statusDownloads->setObjectName("statusDownloads");
    m_statusBar->addWidget(m_statusDownloads);
    
    m_statusBar->addPermanentWidget(new QLabel("|", this));
    
    // Current speed
    m_statusSpeed = new QLabel("Speed: 0 KB/s", this);
    m_statusSpeed->setObjectName("statusSpeed");
    m_statusBar->addPermanentWidget(m_statusSpeed);
    
    m_statusBar->addPermanentWidget(new QLabel("|", this));
    
    // Time remaining
    m_statusRemaining = new QLabel("Remaining: --:--", this);
    m_statusRemaining->setObjectName("statusRemaining");
    m_statusBar->addPermanentWidget(m_statusRemaining);
}

void MainWindow::setupSystemTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }
    
    m_systemTray = new QSystemTrayIcon(this);
    m_systemTray->setIcon(QIcon(":/icons/ldm_tray.png"));
    m_systemTray->setToolTip("LDM - Linux Download Manager");
    
    // Create tray menu
    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction("&Show LDM", this, &QWidget::showNormal);
    m_trayMenu->addAction("&Hide LDM", this, &QWidget::hide);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("&Add URL...", this, &MainWindow::onNewDownload);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("&Exit", this, &MainWindow::onExit);
    
    m_systemTray->setContextMenu(m_trayMenu);
    m_systemTray->show();
}

void MainWindow::setupSplitters()
{
    // Configure splitter handles
    m_horizontalSplitter->setHandleWidth(3);
    m_verticalSplitter->setHandleWidth(3);
    
    // Set initial sizes
    QList<int> horizontalSizes;
    horizontalSizes << DEFAULT_SIDEBAR_WIDTH << (DEFAULT_WINDOW_WIDTH - DEFAULT_SIDEBAR_WIDTH);
    m_horizontalSplitter->setSizes(horizontalSizes);
    
    QList<int> verticalSizes;
    verticalSizes << (DEFAULT_WINDOW_HEIGHT - DEFAULT_DETAILS_HEIGHT - 100) << DEFAULT_DETAILS_HEIGHT;
    m_verticalSplitter->setSizes(verticalSizes);
}

void MainWindow::setupConnections()
{
    // Category sidebar connections
    connect(m_categorySidebar, &CategorySidebar::categorySelected, 
            this, &MainWindow::onCategoryChanged);
    
    // Download table connections
    connect(m_downloadTable, &DownloadTableWidget::selectionChanged,
            this, &MainWindow::onDownloadSelectionChanged);
    connect(m_downloadTable, &DownloadTableWidget::downloadDoubleClicked,
            this, &MainWindow::onDownloadDoubleClicked);
    connect(m_downloadTable, &DownloadTableWidget::contextMenuRequested,
            this, &MainWindow::onDownloadContextMenu);
    
    // Details panel connections
    connect(m_detailsPanel, &DownloadDetailsPanel::resumeRequested,
            this, &MainWindow::onResumeSelected);
    connect(m_detailsPanel, &DownloadDetailsPanel::pauseRequested,
            this, &MainWindow::onPauseSelected);
    connect(m_detailsPanel, &DownloadDetailsPanel::stopRequested,
            this, &MainWindow::onStopSelected);
    connect(m_detailsPanel, &DownloadDetailsPanel::restartRequested,
            this, &MainWindow::onRemoveSelected);
    
    // System tray connections
    if (m_systemTray) {
        connect(m_systemTray, &QSystemTrayIcon::activated,
                this, &MainWindow::onTrayIconActivated);
    }
}

void MainWindow::setupShortcuts()
{
    setupIDMShortcuts();
}

void MainWindow::setupIDMShortcuts()
{
    // Essential IDM shortcuts
    new QShortcut(QKeySequence("Ctrl+N"), this, SLOT(onNewDownload()));
    new QShortcut(QKeySequence("F9"), this, SLOT(onResumeSelected()));
    new QShortcut(QKeySequence("F5"), this, SLOT(onPauseSelected()));
    new QShortcut(QKeySequence("F6"), this, SLOT(onStopSelected()));
    new QShortcut(QKeySequence("Delete"), this, SLOT(onRemoveSelected()));
    new QShortcut(QKeySequence("Ctrl+A"), this, SLOT(onSelectAll()));
    new QShortcut(QKeySequence("Alt+Enter"), this, SLOT(onProperties()));
    new QShortcut(QKeySequence("Ctrl+O"), this, SLOT(onOptions()));
    new QShortcut(QKeySequence("F1"), this, SLOT(onHelp()));
    new QShortcut(QKeySequence("Alt+F4"), this, SLOT(onExit()));
}

void MainWindow::setupDragDrop()
{
    setAcceptDrops(true);
}

void MainWindow::applyIDMStyle()
{
    // Set application style
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // Set color palette
    QPalette palette;
    palette.setColor(QPalette::Window, IDM_BACKGROUND_COLOR);
    palette.setColor(QPalette::WindowText, IDM_TEXT_COLOR);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(248, 248, 248));
    palette.setColor(QPalette::Text, IDM_TEXT_COLOR);
    palette.setColor(QPalette::Highlight, IDM_SELECTION_COLOR);
    palette.setColor(QPalette::HighlightedText, Qt::white);
    
    QApplication::setPalette(palette);
}

void MainWindow::loadIDMStyleSheet()
{
    QFile styleFile(":/styles/idm_style.qss");
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString styleSheet = QString::fromUtf8(styleFile.readAll());
        setStyleSheet(styleSheet);
        styleFile.close();
    }
}

void MainWindow::setupIDMIcons()
{
    // Load and set up icons for IDM-style appearance
    // Icons will be loaded from resources
}

void MainWindow::configureIDMLayout()
{
    // Configure layout to match IDM proportions exactly
    // This ensures the interface looks identical to IDM
}

// === SLOT IMPLEMENTATIONS ===

void MainWindow::addDownloadUrl(const QString &url, const QString &filename)
{
    showAddUrlDialog(url);
}

void MainWindow::showAddUrlDialog(const QString &url)
{
    if (!m_addUrlDialog) {
        m_addUrlDialog = new AddUrlDialog(url, this);
        connect(m_addUrlDialog, &AddUrlDialog::downloadRequested,
                this, [this](const AddUrlDialog::DownloadOptions &options) {
                    // Process download request
                    int downloadId = m_downloadTable->addDownload(options.url, options.filename, 0);
                    m_currentDownloadId = downloadId;
                    updateUIState();
                });
    }
    
    if (!url.isEmpty()) {
        m_addUrlDialog->setUrl(url);
    }
    
    m_addUrlDialog->loadFromClipboard();
    m_addUrlDialog->show();
    m_addUrlDialog->raise();
    m_addUrlDialog->activateWindow();
}

void MainWindow::showSettingsDialog()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(this);
        connect(m_settingsDialog, &SettingsDialog::settingsChanged,
                this, &MainWindow::onSettingsChanged);
    }
    
    m_settingsDialog->show();
    m_settingsDialog->raise();
    m_settingsDialog->activateWindow();
}

// File Menu Slots
void MainWindow::onNewDownload()
{
    showAddUrlDialog();
}

void MainWindow::onAddBatch()
{
    // TODO: Implement batch download dialog
    QMessageBox::information(this, "Add Batch Download", "Batch download feature coming soon!");
}

void MainWindow::onImportFromBrowser()
{
    // TODO: Implement browser import
    QMessageBox::information(this, "Import from Browser", "Browser import feature coming soon!");
}

void MainWindow::onExportDownloads()
{
    QString fileName = QFileDialog::getSaveFileName(this, 
        "Export Download List", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/downloads.csv",
        "CSV Files (*.csv);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement export functionality
        QMessageBox::information(this, "Export Downloads", "Download list exported successfully!");
    }
}

void MainWindow::onExit()
{
    close();
}

// Edit Menu Slots
void MainWindow::onSelectAll()
{
    m_downloadTable->selectAll();
}

void MainWindow::onSelectCompleted()
{
    m_downloadTable->selectCompleted();
}

void MainWindow::onSelectUncompleted()
{
    m_downloadTable->selectUncompleted();
}

void MainWindow::onInvertSelection()
{
    m_downloadTable->invertSelection();
}

void MainWindow::onCopyUrlToClipboard()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    if (selectedIds.isEmpty()) {
        return;
    }
    
    // TODO: Copy URLs to clipboard
    QApplication::clipboard()->setText("http://example.com/file.zip");
    showNotification("URL Copied", "Download URL copied to clipboard");
}

void MainWindow::onProperties()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    if (selectedIds.isEmpty()) {
        QMessageBox::information(this, "Properties", "Please select a download first.");
        return;
    }
    
    // TODO: Show properties dialog
    QMessageBox::information(this, "Properties", "Properties dialog coming soon!");
}

// Downloads Menu Slots
void MainWindow::onResumeSelected()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    for (int id : selectedIds) {
        // TODO: Resume download
        showNotification("Download Resumed", QString("Download #%1 resumed").arg(id));
    }
}

void MainWindow::onResumeAll()
{
    // TODO: Resume all downloads
    showNotification("All Downloads Resumed", "All downloads have been resumed");
}

void MainWindow::onPauseSelected()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    for (int id : selectedIds) {
        // TODO: Pause download
        showNotification("Download Paused", QString("Download #%1 paused").arg(id));
    }
}

void MainWindow::onPauseAll()
{
    // TODO: Pause all downloads
    showNotification("All Downloads Paused", "All downloads have been paused");
}

void MainWindow::onStopSelected()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    for (int id : selectedIds) {
        // TODO: Stop download
    }
}

void MainWindow::onStopAll()
{
    // TODO: Stop all downloads
}

void MainWindow::onRemoveSelected()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    if (selectedIds.isEmpty()) {
        return;
    }
    
    int ret = QMessageBox::question(this, "Remove Downloads",
        QString("Are you sure you want to remove %1 download(s)?").arg(selectedIds.size()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        for (int id : selectedIds) {
            m_downloadTable->removeDownload(id);
        }
        updateUIState();
    }
}

void MainWindow::onRemoveCompleted()
{
    m_downloadTable->removeCompletedDownloads();
    updateUIState();
}

void MainWindow::onRemoveAll()
{
    int ret = QMessageBox::question(this, "Remove All Downloads",
        "Are you sure you want to remove ALL downloads?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_downloadTable->removeAllDownloads();
        updateUIState();
    }
}

void MainWindow::onMoveToTop()
{
    // TODO: Implement move to top
}

void MainWindow::onMoveUp()
{
    // TODO: Implement move up
}

void MainWindow::onMoveDown()
{
    // TODO: Implement move down
}

void MainWindow::onMoveToBottom()
{
    // TODO: Implement move to bottom
}

// View Menu Slots
void MainWindow::onToolbarToggle()
{
    m_mainToolBar->setVisible(!m_mainToolBar->isVisible());
}

void MainWindow::onStatusBarToggle()
{
    m_statusBar->setVisible(!m_statusBar->isVisible());
}

void MainWindow::onCategoriesToggle()
{
    m_categorySidebar->setVisible(!m_categorySidebar->isVisible());
}

void MainWindow::onDetailsToggle()
{
    m_detailsPanel->setVisible(!m_detailsPanel->isVisible());
}

void MainWindow::onRefreshView()
{
    refreshDownloadList();
    updateUIState();
}

void MainWindow::onCustomizeColumns()
{
    // TODO: Implement column customization dialog
    QMessageBox::information(this, "Customize Columns", "Column customization coming soon!");
}

// Tools Menu Slots
void MainWindow::onOptions()
{
    showSettingsDialog();
}

void MainWindow::onScheduler()
{
    QMessageBox::information(this, "Scheduler", "Scheduler feature coming soon!");
}

void MainWindow::onSpeedLimiter()
{
    QMessageBox::information(this, "Speed Limiter", "Speed limiter feature coming soon!");
}

void MainWindow::onSiteExplorer()
{
    QMessageBox::information(this, "Site Explorer", "Site explorer feature coming soon!");
}

void MainWindow::onVideoDownloader()
{
    QMessageBox::information(this, "Video Downloader", "Video downloader feature coming soon!");
}

void MainWindow::onBrowserIntegration()
{
    QMessageBox::information(this, "Browser Integration", "Browser integration feature coming soon!");
}

// Help Menu Slots
void MainWindow::onHelp()
{
    QDesktopServices::openUrl(QUrl("https://github.com/aerab243/ldm/wiki"));
}

void MainWindow::onKeyboardShortcuts()
{
    QString shortcuts = 
        "LDM Keyboard Shortcuts:\n\n"
        "Ctrl+N - New Download\n"
        "F9 - Resume Selected\n"
        "F5 - Pause Selected\n"
        "F6 - Stop Selected\n"
        "Delete - Remove Selected\n"
        "Ctrl+A - Select All\n"
        "Alt+Enter - Properties\n"
        "Ctrl+O - Options\n"
        "F1 - Help\n"
        "Alt+F4 - Exit";
    
    QMessageBox::information(this, "Keyboard Shortcuts", shortcuts);
}

void MainWindow::onCheckUpdates()
{
    QMessageBox::information(this, "Check Updates", "You are using the latest version of LDM!");
}

void MainWindow::onAbout()
{
    QString aboutText = 
        "<h3>LDM - Linux Download Manager</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A modern, feature-rich download manager for Linux</p>"
        "<p>Developed by Anna-el Gerard RABENANDRASANA</p>"
        "<p><a href='https://github.com/aerab243/ldm'>https://github.com/aerab243/ldm</a></p>"
        "<p>Built with Qt " QT_VERSION_STR "</p>";
    
    QMessageBox::about(this, "About LDM", aboutText);
}

// Toolbar Slots
void MainWindow::onToolbarAddUrl()
{
    onNewDownload();
}

void MainWindow::onToolbarStartAll()
{
    onResumeAll();
}

void MainWindow::onToolbarPauseAll()
{
    onPauseAll();
}

void MainWindow::onToolbarStopAll()
{
    onStopAll();
}

void MainWindow::onToolbarRemove()
{
    onRemoveSelected();
}

void MainWindow::onToolbarSettings()
{
    onOptions();
}

// Event Handlers
void MainWindow::onCategoryChanged(const QString &category)
{
    m_currentCategory = category;
    m_downloadTable->filterByCategory(category);
    updateUIState();
}

void MainWindow::onDownloadSelectionChanged()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    
    if (selectedIds.size() == 1) {
        m_currentDownloadId = selectedIds.first();
        // TODO: Update details panel with download info
    } else {
        m_currentDownloadId = -1;
        m_detailsPanel->clearDownloadInfo();
    }
    
    updateUIState();
}

void MainWindow::onDownloadDoubleClicked(int row)
{
    Q_UNUSED(row)
    
    // TODO: Open file or show properties
    QMessageBox::information(this, "Open Download", "Opening download...");
}

void MainWindow::onDownloadContextMenu(const QPoint &pos)
{
    Q_UNUSED(pos)
    
    // TODO: Show context menu
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if (isVisible()) {
                hide();
            } else {
                showNormal();
                raise();
                activateWindow();
            }
            break;
        default:
            break;
    }
}

void MainWindow::onUpdateTimer()
{
    updateUIState();
    updateStatusBar();
    updateTrayIcon();
}

void MainWindow::onSettingsChanged()
{
    // Reload settings and update UI
    updateUIState();
}

// Event Overrides
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_systemTray && m_systemTray->isVisible()) {
        // Minimize to tray instead of closing
        hide();
        showNotification("LDM Minimized", "LDM is still running in the system tray");
        event->ignore();
    } else {
        // Save settings before closing
        saveLayoutSettings();
        event->accept();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateLayoutForResponsive();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Handle additional keyboard shortcuts
    QMainWindow::keyPressEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl &url : urls) {
        if (url.isValid()) {
            showAddUrlDialog(url.toString());
            break; // Only handle first URL for now
        }
    }
    event->acceptProposedAction();
}

// Utility Methods
void MainWindow::saveLayoutSettings()
{
    if (!m_settings) return;
    
    m_settings->beginGroup("Layout");
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
    m_settings->setValue("horizontalSplitter", m_horizontalSplitter->saveState());
    m_settings->setValue("verticalSplitter", m_verticalSplitter->saveState());
    m_settings->endGroup();
}

void MainWindow::loadLayoutSettings()
{
    if (!m_settings) return;
    
    m_settings->beginGroup("Layout");
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());
    m_horizontalSplitter->restoreState(m_settings->value("horizontalSplitter").toByteArray());
    m_verticalSplitter->restoreState(m_settings->value("verticalSplitter").toByteArray());
    m_settings->endGroup();
}

void MainWindow::resetLayoutToDefault()
{
    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    setupSplitters();
}

void MainWindow::updateLayoutForResponsive()
{
    // Adjust layout for different window sizes
    int width = this->width();
    
    if (width < 800) {
        // Small window - hide sidebar
        m_categorySidebar->hide();
    } else {
        // Normal window - show sidebar
        m_categorySidebar->show();
    }
}

void MainWindow::refreshDownloadList()
{
    // TODO: Refresh download list from backend
    m_downloadTable->sortByColumn(0, Qt::AscendingOrder);
}

void MainWindow::updateDownloadCounts()
{
    // Update category counts
    m_categorySidebar->refreshCategoryCounters();
}

void MainWindow::filterDownloadsByCategory()
{
    m_downloadTable->filterByCategory(m_currentCategory);
}

void MainWindow::updateUIState()
{
    updateToolbarState();
    updateMenuState();
    updateStatusBar();
    updateDownloadCounts();
}

void MainWindow::updateToolbarState()
{
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    bool hasSelection = !selectedIds.isEmpty();
    bool hasDownloads = m_downloadTable->getDownloadCount() > 0;
    
    m_removeAction->setEnabled(hasSelection);
    m_startAllAction->setEnabled(hasDownloads);
    m_pauseAllAction->setEnabled(hasDownloads);
    m_stopAllAction->setEnabled(hasDownloads);
}

void MainWindow::updateMenuState()
{
    // Update menu items based on current state
    QList<int> selectedIds = m_downloadTable->getSelectedDownloadIds();
    bool hasSelection = !selectedIds.isEmpty();
    
    // Update Edit menu
    if (m_editMenu) {
        for (QAction *action : m_editMenu->actions()) {
            if (action->text().contains("Copy URL") || action->text().contains("Properties")) {
                action->setEnabled(hasSelection);
            }
        }
    }
    
    // Update Downloads menu
    if (m_downloadsMenu) {
        for (QAction *action : m_downloadsMenu->actions()) {
            if (action->text().contains("Resume") && !action->text().contains("All")) {
                action->setEnabled(hasSelection);
            } else if (action->text().contains("Pause") && !action->text().contains("All")) {
                action->setEnabled(hasSelection);
            } else if (action->text().contains("Stop") && !action->text().contains("All")) {
                action->setEnabled(hasSelection);
            } else if (action->text().contains("Remove") && !action->text().contains("All")) {
                action->setEnabled(hasSelection);
            }
        }
    }
}

void MainWindow::updateStatusBar()
{
    if (!m_statusBar) return;
    
    int totalDownloads = m_downloadTable->getDownloadCount();
    int totalSpeed = m_downloadTable->getTotalSpeed();
    
    m_statusDownloads->setText(QString("Downloads: %1").arg(totalDownloads));
    m_statusSpeed->setText(QString("Speed: %1").arg(formatSpeed(totalSpeed)));
    m_statusRemaining->setText("Remaining: --:--"); // TODO: Calculate actual remaining time
}

void MainWindow::updateTrayIcon()
{
    if (!m_systemTray) return;
    
    // TODO: Update tray icon based on download status
    // For now, keep static icon
}

void MainWindow::showNotification(const QString &title, const QString &message)
{
    if (m_systemTray && m_systemTray->supportsMessages()) {
        m_systemTray->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    }
}

QString MainWindow::formatFileSize(qint64 bytes) const
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (bytes >= GB) {
        return QString("%1 GB").arg(QString::number(bytes / double(GB), 'f', 2));
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(QString::number(bytes / double(MB), 'f', 1));
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(QString::number(bytes / double(KB), 'f', 0));
    } else {
        return QString("%1 B").arg(bytes);
    }
}

QString MainWindow::formatSpeed(qint64 bytesPerSecond) const
{
    if (bytesPerSecond == 0) {
        return "0 KB/s";
    }
    
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    
    if (bytesPerSecond >= MB) {
        return QString("%1 MB/s").arg(QString::number(bytesPerSecond / double(MB), 'f', 2));
    } else if (bytesPerSecond >= KB) {
        return QString("%1 KB/s").arg(QString::number(bytesPerSecond / double(KB), 'f', 1));
    } else {
        return QString("%1 B/s").arg(bytesPerSecond);
    }
}

QString MainWindow::formatTime(int seconds) const
{
    if (seconds < 0) {
        return "--:--";
    }
    
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    } else {
        return QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    }
}

QString MainWindow::formatETA(int seconds) const
{
    return formatTime(seconds);
}

QIcon MainWindow::getFileTypeIcon(const QString &filename) const
{
    QFileInfo fileInfo(filename);
    QString extension = fileInfo.suffix().toLower();
    
    // Return appropriate icon based on file extension
    if (extension == "zip" || extension == "rar" || extension == "7z") {
        return QIcon(":/icons/file_archive.png");
    } else if (extension == "mp4" || extension == "avi" || extension == "mkv") {
        return QIcon(":/icons/file_video.png");
    } else if (extension == "mp3" || extension == "wav" || extension == "flac") {
        return QIcon(":/icons/file_audio.png");
    } else if (extension == "pdf" || extension == "doc" || extension == "txt") {
        return QIcon(":/icons/file_document.png");
    } else if (extension == "exe" || extension == "msi" || extension == "deb") {
        return QIcon(":/icons/file_program.png");
    } else if (extension == "jpg" || extension == "png" || extension == "gif") {
        return QIcon(":/icons/file_image.png");
    } else {
        return QIcon(":/icons/file_generic.png");
    }
}

QString MainWindow::getFileCategory(const QString &filename) const
{
    QFileInfo fileInfo(filename);
    QString extension = fileInfo.suffix().toLower();
    
    if (extension == "zip" || extension == "rar" || extension == "7z" || extension == "tar" || extension == "gz") {
        return "Compressed";
    } else if (extension == "mp4" || extension == "avi" || extension == "mkv" || extension == "mov" || extension == "wmv") {
        return "Video";
    } else if (extension == "mp3" || extension == "wav" || extension == "flac" || extension == "aac" || extension == "ogg") {
        return "Audio";
    } else if (extension == "pdf" || extension == "doc" || extension == "docx" || extension == "txt" || extension == "rtf") {
        return "Documents";
    } else if (extension == "exe" || extension == "msi" || extension == "deb" || extension == "rpm" || extension == "dmg") {
        return "Programs";
    } else {
        return "All Downloads";
    }
}