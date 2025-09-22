#include "MainWindow.h"
#include "ui/AddUrlDialog.h"
#include "ui/CategoryManager.h"
#include "ui/ThemeManager.h"
#include "ui/SettingsDialog.h"
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QAction>
#include <QCloseEvent>
#include <QPropertyAnimation>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QSplitter>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include "core/DownloadEngine.h"
#include "core/Database.h"
#include "api/ApiServer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialize core components to nullptr first
    m_downloadEngine = nullptr;
    m_database = nullptr;
    m_apiServer = nullptr;

    setupUI();
    setupTrayIcon();
    setupConnections();
    
    // Load existing downloads (sample data for now)
    loadDownloads();

    // Apply theme
    ThemeManager::instance()->setTheme(ThemeManager::Light);
    
    // Fade in animation
    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(300);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    
    // TODO: Initialize core components after UI is stable
    // m_downloadEngine = new DownloadEngine(this);
    // m_database = new Database(this);
    // m_apiServer = new ApiServer(m_database, m_downloadEngine, this);
    // m_apiServer->start();
}

MainWindow::~MainWindow()
{
    // Qt handles deletion automatically
}

void MainWindow::setupUI()
{
    setWindowTitle("LDM - Like Download Manager 1.0.0");
    setMinimumSize(1000, 700);
    resize(1200, 800);

    setupMenuBar();
    setupToolBar();
    
    // Create central widget with splitters
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(2);

    // Create horizontal splitter for categories and downloads
    m_horizontalSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Create vertical splitter for downloads table and details
    m_verticalSplitter = new QSplitter(Qt::Vertical, this);

    setupCategoriesPanel();
    setupDownloadsTable();
    setupDetailsPanel();

    // Add downloads table and details to vertical splitter
    m_verticalSplitter->addWidget(m_downloadsTable);
    m_verticalSplitter->addWidget(m_detailsGroup);
    m_verticalSplitter->setSizes({500, 200});

    // Add categories and vertical splitter to horizontal splitter
    m_horizontalSplitter->addWidget(m_categoriesTree);
    m_horizontalSplitter->addWidget(m_verticalSplitter);
    m_horizontalSplitter->setSizes({200, 800});

    mainLayout->addWidget(m_horizontalSplitter);

    setupStatusBar();
    populateCategories();
    
    // Apply IDM-style styling
    applyIDMStyling();
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // Tasks menu
    QMenu *tasksMenu = menuBar->addMenu("&Tasks");
    QAction *addUrlAction = tasksMenu->addAction("&Add URL...");
    addUrlAction->setShortcut(QKeySequence("Ctrl+N"));
    tasksMenu->addSeparator();
    QAction *exitAction = tasksMenu->addAction("E&xit");
    
    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    QAction *importAction = fileMenu->addAction("&Import...");
    QAction *exportAction = fileMenu->addAction("&Export...");
    fileMenu->addSeparator();
    QAction *propertiesAction = fileMenu->addAction("&Properties");
    
    // Downloads menu
    QMenu *downloadsMenu = menuBar->addMenu("&Downloads");
    QAction *resumeAction = downloadsMenu->addAction("&Resume");
    QAction *pauseAction = downloadsMenu->addAction("&Pause");
    QAction *stopAction = downloadsMenu->addAction("&Stop");
    downloadsMenu->addSeparator();
    QAction *deleteAction = downloadsMenu->addAction("&Delete");
    QAction *deleteCompletedAction = downloadsMenu->addAction("Delete &Completed");
    
    // View menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    QAction *toolbarAction = viewMenu->addAction("&Toolbar");
    toolbarAction->setCheckable(true);
    toolbarAction->setChecked(true);
    QAction *statusBarAction = viewMenu->addAction("&Status Bar");
    statusBarAction->setCheckable(true);
    statusBarAction->setChecked(true);
    viewMenu->addSeparator();
    QAction *categoriesAction = viewMenu->addAction("&Categories");
    categoriesAction->setCheckable(true);
    categoriesAction->setChecked(true);
    
    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    QAction *aboutAction = helpMenu->addAction("&About LDM");
    
    // Connect actions
    connect(addUrlAction, &QAction::triggered, this, &MainWindow::onAddUrlClicked);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(resumeAction, &QAction::triggered, this, &MainWindow::onResumeClicked);
    connect(stopAction, &QAction::triggered, this, &MainWindow::onStopClicked);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteClicked);
    connect(deleteCompletedAction, &QAction::triggered, this, &MainWindow::onDeleteCompletedClicked);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
    
    connect(toolbarAction, &QAction::toggled, m_mainToolBar, &QToolBar::setVisible);
    connect(statusBarAction, &QAction::toggled, statusBar(), &QStatusBar::setVisible);
    connect(categoriesAction, &QAction::toggled, m_categoriesTree, &QTreeWidget::setVisible);
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar("Main");
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_mainToolBar->setIconSize(QSize(32, 32));

    // Add URL - Use text for now, icons can be added later
    QAction *addUrlAction = m_mainToolBar->addAction("Add URL");
    connect(addUrlAction, &QAction::triggered, this, &MainWindow::onAddUrlClicked);

    // Resume
    QAction *resumeAction = m_mainToolBar->addAction("Resume");
    connect(resumeAction, &QAction::triggered, this, &MainWindow::onResumeClicked);

    // Stop
    QAction *stopAction = m_mainToolBar->addAction("Stop");
    connect(stopAction, &QAction::triggered, this, &MainWindow::onStopClicked);

    // Stop All
    QAction *stopAllAction = m_mainToolBar->addAction("Stop All");
    connect(stopAllAction, &QAction::triggered, this, &MainWindow::onStopAllClicked);

    // Delete
    QAction *deleteAction = m_mainToolBar->addAction("Delete");
    connect(deleteAction, &QAction::triggered, this, &MainWindow::onDeleteClicked);

    // Delete Completed
    QAction *deleteCompletedAction = m_mainToolBar->addAction("Delete C...");
    connect(deleteCompletedAction, &QAction::triggered, this, &MainWindow::onDeleteCompletedClicked);

    m_mainToolBar->addSeparator();

    // Options
    QAction *optionsAction = m_mainToolBar->addAction("Options");
    connect(optionsAction, &QAction::triggered, this, &MainWindow::onOptionsClicked);

    // Scheduler
    QAction *schedulerAction = m_mainToolBar->addAction("Scheduler");
    connect(schedulerAction, &QAction::triggered, this, &MainWindow::onSchedulerClicked);

    m_mainToolBar->addSeparator();

    // Start Queue
    QAction *startQueueAction = m_mainToolBar->addAction("Start Qu...");
    connect(startQueueAction, &QAction::triggered, this, &MainWindow::onStartQueueClicked);

    // Stop Queue
    QAction *stopQueueAction = m_mainToolBar->addAction("Stop Qu...");
    connect(stopQueueAction, &QAction::triggered, this, &MainWindow::onStopQueueClicked);

    // Grabber
    QAction *grabberAction = m_mainToolBar->addAction("Grabber");
    connect(grabberAction, &QAction::triggered, this, &MainWindow::onGrabberClicked);

    m_mainToolBar->addSeparator();

    // Tell a Friend
    QAction *tellFriendAction = m_mainToolBar->addAction("Tell a Frie...");
    connect(tellFriendAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
}

void MainWindow::setupCategoriesPanel()
{
    m_categoriesTree = new QTreeWidget(this);
    m_categoriesTree->setHeaderLabel("Categories");
    m_categoriesTree->setMaximumWidth(250);
    m_categoriesTree->setMinimumWidth(180);
    m_categoriesTree->setRootIsDecorated(false);
    m_categoriesTree->setAlternatingRowColors(true);
}

void MainWindow::setupDownloadsTable()
{
    m_downloadsTable = new QTableWidget(this);
    
    // Set up columns
    QStringList headers;
    headers << "File Name" << "Size" << "Status" << "Time left" << "Transfer rate" << "Last Try" << "Description";
    m_downloadsTable->setColumnCount(headers.size());
    m_downloadsTable->setHorizontalHeaderLabels(headers);
    
    // Configure table
    m_downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_downloadsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_downloadsTable->setAlternatingRowColors(true);
    m_downloadsTable->setSortingEnabled(true);
    m_downloadsTable->verticalHeader()->setVisible(false);
    
    // Set column widths
    QHeaderView *header = m_downloadsTable->horizontalHeader();
    header->setStretchLastSection(true);
    m_downloadsTable->setColumnWidth(0, 200); // File Name
    m_downloadsTable->setColumnWidth(1, 80);  // Size
    m_downloadsTable->setColumnWidth(2, 100); // Status
    m_downloadsTable->setColumnWidth(3, 80);  // Time left
    m_downloadsTable->setColumnWidth(4, 100); // Transfer rate
    m_downloadsTable->setColumnWidth(5, 120); // Last Try
}

void MainWindow::setupDetailsPanel()
{
    m_detailsGroup = new QGroupBox("Download status", this);
    m_detailsGroup->setMaximumHeight(200);
    
    QGridLayout *detailsLayout = new QGridLayout(m_detailsGroup);
    
    // URL and status info
    QLabel *urlLabel = new QLabel("URL:", this);
    m_detailsUrlLabel = new QLabel("Select a download to view details", this);
    m_detailsUrlLabel->setWordWrap(true);
    
    QLabel *statusLabel = new QLabel("Status:", this);
    m_detailsStatusLabel = new QLabel("Ready", this);
    
    QLabel *fileSizeLabel = new QLabel("File size:", this);
    m_detailsSizeLabel = new QLabel("Unknown", this);
    
    QLabel *downloadedLabel = new QLabel("Downloaded:", this);
    QLabel *downloadedValueLabel = new QLabel("0 MB (0.00 %)", this);
    
    QLabel *transferRateLabel = new QLabel("Transfer rate:", this);
    m_detailsSpeedLabel = new QLabel("0 MB/sec", this);
    
    QLabel *timeLeftLabel = new QLabel("Time left:", this);
    m_detailsTimeLeftLabel = new QLabel("Unknown", this);
    
    QLabel *resumeCapabilityLabel = new QLabel("Resume capability:", this);
    QLabel *resumeValueLabel = new QLabel("Yes", this);
    
    // Progress bar
    m_detailsProgressBar = new QProgressBar(this);
    m_detailsProgressBar->setMinimum(0);
    m_detailsProgressBar->setMaximum(100);
    m_detailsProgressBar->setValue(0);
    m_detailsProgressBar->setTextVisible(false);
    
    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    m_detailsShowDetailsButton = new QPushButton("Show details >>", this);
    m_detailsPauseButton = new QPushButton("Pause", this);
    m_detailsCancelButton = new QPushButton("Cancel", this);
    
    buttonsLayout->addWidget(m_detailsShowDetailsButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_detailsPauseButton);
    buttonsLayout->addWidget(m_detailsCancelButton);
    
    // Layout
    detailsLayout->addWidget(urlLabel, 0, 0);
    detailsLayout->addWidget(m_detailsUrlLabel, 0, 1, 1, 3);
    detailsLayout->addWidget(statusLabel, 1, 0);
    detailsLayout->addWidget(m_detailsStatusLabel, 1, 1);
    detailsLayout->addWidget(fileSizeLabel, 2, 0);
    detailsLayout->addWidget(m_detailsSizeLabel, 2, 1);
    detailsLayout->addWidget(downloadedLabel, 2, 2);
    detailsLayout->addWidget(downloadedValueLabel, 2, 3);
    detailsLayout->addWidget(transferRateLabel, 3, 0);
    detailsLayout->addWidget(m_detailsSpeedLabel, 3, 1);
    detailsLayout->addWidget(timeLeftLabel, 3, 2);
    detailsLayout->addWidget(m_detailsTimeLeftLabel, 3, 3);
    detailsLayout->addWidget(resumeCapabilityLabel, 4, 0);
    detailsLayout->addWidget(resumeValueLabel, 4, 1);
    detailsLayout->addWidget(m_detailsProgressBar, 5, 0, 1, 4);
    detailsLayout->addLayout(buttonsLayout, 6, 0, 1, 4);
}

void MainWindow::setupStatusBar()
{
    // Global progress bar
    m_globalProgressBar = new QProgressBar(this);
    m_globalProgressBar->setMaximumWidth(200);
    m_globalProgressBar->setVisible(false);
    
    // Status label
    m_statusLabel = new QLabel("Ready", this);
    
    // Global speed label
    m_globalSpeedLabel = new QLabel("", this);
    
    statusBar()->addWidget(m_statusLabel);
    statusBar()->addPermanentWidget(m_globalSpeedLabel);
    statusBar()->addPermanentWidget(m_globalProgressBar);
}

void MainWindow::setupTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayMenu = new QMenu(this);

    QAction *showAction = m_trayMenu->addAction("Show");
    QAction *hideAction = m_trayMenu->addAction("Hide");
    m_trayMenu->addSeparator();
    QAction *quitAction = m_trayMenu->addAction("Quit");

    m_trayIcon->setContextMenu(m_trayMenu);

    connect(showAction, &QAction::triggered, this, &QWidget::show);
    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);

    m_trayIcon->show();
}

void MainWindow::setupConnections()
{
    // Categories tree
    connect(m_categoriesTree, &QTreeWidget::currentItemChanged, 
            this, &MainWindow::onCategorySelectionChanged);
    
    // Downloads table
    connect(m_downloadsTable, &QTableWidget::currentCellChanged,
            this, &MainWindow::onDownloadSelectionChanged);
    connect(m_downloadsTable, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::onDownloadDoubleClicked);
    
    // Details panel buttons
    connect(m_detailsPauseButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(m_detailsCancelButton, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);

    // Download engine (connect only if initialized)
    if (m_downloadEngine) {
        connect(m_downloadEngine, &DownloadEngine::downloadProgress, 
                this, &MainWindow::onDownloadProgress);
        connect(m_downloadEngine, &DownloadEngine::downloadCompleted, 
                this, &MainWindow::onDownloadCompleted);
        connect(m_downloadEngine, &DownloadEngine::downloadFailed, 
                this, &MainWindow::onDownloadFailed);
    }
}

void MainWindow::populateCategories()
{
    QTreeWidgetItem *allDownloads = new QTreeWidgetItem(m_categoriesTree);
    allDownloads->setText(0, "📁 All Downloads");
    
    QTreeWidgetItem *compressed = new QTreeWidgetItem(m_categoriesTree);
    compressed->setText(0, "📦 Compressed");
    
    QTreeWidgetItem *documents = new QTreeWidgetItem(m_categoriesTree);
    documents->setText(0, "📄 Documents");
    
    QTreeWidgetItem *music = new QTreeWidgetItem(m_categoriesTree);
    music->setText(0, "🎵 Music");
    
    QTreeWidgetItem *programs = new QTreeWidgetItem(m_categoriesTree);
    programs->setText(0, "⚙ Programs");
    
    QTreeWidgetItem *video = new QTreeWidgetItem(m_categoriesTree);
    video->setText(0, "🎬 Video");
    
    QTreeWidgetItem *unfinished = new QTreeWidgetItem(m_categoriesTree);
    unfinished->setText(0, "⏳ Unfinished");
    
    QTreeWidgetItem *finished = new QTreeWidgetItem(m_categoriesTree);
    finished->setText(0, "✓ Finished");
    
    QTreeWidgetItem *queues = new QTreeWidgetItem(m_categoriesTree);
    queues->setText(0, "📋 Queues");
    
    m_categoriesTree->setCurrentItem(allDownloads);
}

void MainWindow::loadDownloads()
{
    // TODO: Load from database and populate table
    // For now, add some sample data to demonstrate the interface
    
    // Sample download 1
    int row = m_downloadsTable->rowCount();
    m_downloadsTable->insertRow(row);
    
    QTableWidgetItem *nameItem = new QTableWidgetItem("♪ undercover.mp3");
    m_downloadsTable->setItem(row, 0, nameItem);
    
    m_downloadsTable->setItem(row, 1, new QTableWidgetItem("7.16 MB"));
    m_downloadsTable->setItem(row, 2, new QTableWidgetItem("Complete"));
    m_downloadsTable->setItem(row, 3, new QTableWidgetItem(""));
    m_downloadsTable->setItem(row, 4, new QTableWidgetItem(""));
    m_downloadsTable->setItem(row, 5, new QTableWidgetItem("Nov 08 ..."));
    m_downloadsTable->setItem(row, 6, new QTableWidgetItem(""));
    
    // Sample download 2
    row = m_downloadsTable->rowCount();
    m_downloadsTable->insertRow(row);
    
    nameItem = new QTableWidgetItem("🎬 mult.flv");
    m_downloadsTable->setItem(row, 0, nameItem);
    
    m_downloadsTable->setItem(row, 1, new QTableWidgetItem("37.017 MB"));
    m_downloadsTable->setItem(row, 2, new QTableWidgetItem("94.97%"));
    m_downloadsTable->setItem(row, 3, new QTableWidgetItem("0 sec"));
    m_downloadsTable->setItem(row, 4, new QTableWidgetItem("4.67 MB/sec"));
    m_downloadsTable->setItem(row, 5, new QTableWidgetItem("Oct 13 2..."));
    m_downloadsTable->setItem(row, 6, new QTableWidgetItem(""));
}

void MainWindow::saveDownloads()
{
    // Save current downloads to database
    // For now, assume it's handled by DownloadEngine
}

void MainWindow::applyIDMStyling()
{
    setStyleSheet(
        "QMainWindow {"
        "   background-color: #f0f0f0;"
        "}"
        "QToolBar {"
        "   background-color: #e8e8e8;"
        "   border: 1px solid #ccc;"
        "   spacing: 2px;"
        "   padding: 2px;"
        "}"
        "QTreeWidget {"
        "   background-color: white;"
        "   border: 1px solid #ccc;"
        "   alternate-background-color: #f5f5f5;"
        "}"
        "QTableWidget {"
        "   background-color: white;"
        "   border: 1px solid #ccc;"
        "   gridline-color: #ddd;"
        "   alternate-background-color: #f9f9f9;"
        "}"
        "QGroupBox {"
        "   font-weight: bold;"
        "   border: 2px solid #ccc;"
        "   border-radius: 5px;"
        "   margin-top: 1ex;"
        "   background-color: #fafafa;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 5px 0 5px;"
        "}"
        "QStatusBar {"
        "   background-color: #e8e8e8;"
        "   border-top: 1px solid #ccc;"
        "}"
        "QProgressBar {"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "   text-align: center;"
        "   background-color: #f0f0f0;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #4CAF50;"
        "   border-radius: 2px;"
        "}"
    );
}

// Slot implementations
void MainWindow::onAddUrlClicked()
{
    if (!m_database) {
        QMessageBox::information(this, "Add URL", 
            "Add URL functionality will be available when core components are initialized.\n\n"
            "For now, this is a demonstration of the IDM-style interface.");
        return;
    }
    
    AddUrlDialog dialog(m_database, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString url = dialog.getUrl();
        QString fileName = dialog.getFileName();
        int categoryId = dialog.getCategoryId();

        // Add to table and database
        DownloadItem item(0, url, fileName);
        addDownloadToTable(item);
        
        QVariantMap data;
        data["url"] = url;
        data["filename"] = fileName;
        data["category_id"] = categoryId;
        m_database->insertDownload(data);
        
        m_statusLabel->setText("Download added: " + fileName);
    }
}

void MainWindow::onResumeClicked()
{
    int currentRow = m_downloadsTable->currentRow();
    if (currentRow >= 0) {
        QTableWidgetItem *nameItem = m_downloadsTable->item(currentRow, 0);
        QString fileName = nameItem ? nameItem->text() : "Unknown";
        m_statusLabel->setText("Resuming download: " + fileName);
        QMessageBox::information(this, "Resume Download", 
            "Resume functionality will be implemented when download engine is integrated.");
    } else {
        QMessageBox::information(this, "Resume Download", "Please select a download to resume.");
    }
}

void MainWindow::onStopClicked()
{
    int currentRow = m_downloadsTable->currentRow();
    if (currentRow >= 0) {
        QTableWidgetItem *nameItem = m_downloadsTable->item(currentRow, 0);
        QString fileName = nameItem ? nameItem->text() : "Unknown";
        m_statusLabel->setText("Stopping download: " + fileName);
        QMessageBox::information(this, "Stop Download", 
            "Stop functionality will be implemented when download engine is integrated.");
    } else {
        QMessageBox::information(this, "Stop Download", "Please select a download to stop.");
    }
}

void MainWindow::onStopAllClicked()
{
    m_statusLabel->setText("Stopping all downloads...");
    // TODO: Stop all downloads
}

void MainWindow::onDeleteClicked()
{
    int currentRow = m_downloadsTable->currentRow();
    if (currentRow >= 0) {
        m_downloadsTable->removeRow(currentRow);
        m_statusLabel->setText("Download deleted.");
    }
}

void MainWindow::onDeleteCompletedClicked()
{
    // TODO: Delete all completed downloads
    m_statusLabel->setText("Completed downloads deleted.");
}

void MainWindow::onOptionsClicked()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onSchedulerClicked()
{
    QMessageBox::information(this, "Scheduler", "Scheduler functionality not implemented yet.");
}

void MainWindow::onStartQueueClicked()
{
    m_statusLabel->setText("Starting download queue...");
    // TODO: Start queue
}

void MainWindow::onStopQueueClicked()
{
    m_statusLabel->setText("Stopping download queue...");
    // TODO: Stop queue
}

void MainWindow::onGrabberClicked()
{
    QMessageBox::information(this, "Grabber", "Media grabber functionality not implemented yet.");
}

void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, "About LDM", 
        "LDM - Like Download Manager\n"
        "Version 1.0.0\n\n"
        "A powerful download manager inspired by IDM.\n\n"
        "Built with Qt 6 and C++20");
}

void MainWindow::onCategorySelectionChanged()
{
    QTreeWidgetItem *current = m_categoriesTree->currentItem();
    if (current) {
        QString category = current->text(0);
        filterDownloadsByCategory(category);
        m_statusLabel->setText("Showing: " + category);
    }
}

void MainWindow::onDownloadSelectionChanged()
{
    updateDetailsPanel();
}

void MainWindow::onDownloadDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row >= 0) {
        // TODO: Open file or show properties
        m_statusLabel->setText("Opening download properties...");
    }
}

void MainWindow::onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal)
{
    updateDownloadInTable(downloadId, bytesReceived, bytesTotal);
    updateDetailsPanel();
}

void MainWindow::onDownloadCompleted(int downloadId)
{
    updateDownloadInTable(downloadId, 0, 0, "Complete");
    showNotification("Download Completed", "Download completed successfully.");
}

void MainWindow::onDownloadFailed(int downloadId, const QString &error)
{
    updateDownloadInTable(downloadId, 0, 0, "Failed: " + error);
    showNotification("Download Failed", "Download failed: " + error);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        show();
        activateWindow();
    }
}

// Utility functions
void MainWindow::addDownloadToTable(const DownloadItem &item)
{
    int row = m_downloadsTable->rowCount();
    m_downloadsTable->insertRow(row);
    
    QTableWidgetItem *nameItem = new QTableWidgetItem(item.getFileName());
    nameItem->setData(Qt::UserRole, item.getId());
    m_downloadsTable->setItem(row, 0, nameItem);
    
    m_downloadsTable->setItem(row, 1, new QTableWidgetItem("Unknown"));
    m_downloadsTable->setItem(row, 2, new QTableWidgetItem("Connecting..."));
    m_downloadsTable->setItem(row, 3, new QTableWidgetItem(""));
    m_downloadsTable->setItem(row, 4, new QTableWidgetItem(""));
    m_downloadsTable->setItem(row, 5, new QTableWidgetItem(QDateTime::currentDateTime().toString("MMM dd ...")));
    m_downloadsTable->setItem(row, 6, new QTableWidgetItem(""));
}

void MainWindow::updateDownloadInTable(int downloadId, qint64 bytesReceived, qint64 bytesTotal, const QString &status)
{
    int row = findDownloadRowById(downloadId);
    if (row >= 0) {
        if (bytesTotal > 0) {
            m_downloadsTable->item(row, 1)->setText(formatFileSize(bytesTotal));
            double progress = (double)bytesReceived / bytesTotal * 100.0;
            m_downloadsTable->item(row, 2)->setText(QString("%1%").arg(progress, 0, 'f', 1));
        }
        if (!status.isEmpty()) {
            m_downloadsTable->item(row, 2)->setText(status);
        }
    }
}

void MainWindow::removeDownloadFromTable(int downloadId)
{
    int row = findDownloadRowById(downloadId);
    if (row >= 0) {
        m_downloadsTable->removeRow(row);
    }
}

int MainWindow::findDownloadRowById(int downloadId)
{
    for (int row = 0; row < m_downloadsTable->rowCount(); ++row) {
        QTableWidgetItem *item = m_downloadsTable->item(row, 0);
        if (item && item->data(Qt::UserRole).toInt() == downloadId) {
            return row;
        }
    }
    return -1;
}

QString MainWindow::getCurrentCategory()
{
    QTreeWidgetItem *current = m_categoriesTree->currentItem();
    return current ? current->text(0) : QString();
}

void MainWindow::filterDownloadsByCategory(const QString &category)
{
    // TODO: Implement category filtering
    Q_UNUSED(category);
}

void MainWindow::updateDownloadsList()
{
    // TODO: Refresh downloads list from database
}

void MainWindow::updateDetailsPanel()
{
    int currentRow = m_downloadsTable->currentRow();
    if (currentRow >= 0) {
        // Update details panel with current selection
        QTableWidgetItem *nameItem = m_downloadsTable->item(currentRow, 0);
        if (nameItem) {
            m_detailsUrlLabel->setText("Selected: " + nameItem->text());
            
            QTableWidgetItem *sizeItem = m_downloadsTable->item(currentRow, 1);
            if (sizeItem) {
                m_detailsSizeLabel->setText(sizeItem->text());
            }
            
            QTableWidgetItem *statusItem = m_downloadsTable->item(currentRow, 2);
            if (statusItem) {
                m_detailsStatusLabel->setText(statusItem->text());
                
                // Try to parse progress percentage
                QString statusText = statusItem->text();
                if (statusText.contains('%')) {
                    QString percentStr = statusText.split('%').first();
                    bool ok;
                    double percent = percentStr.toDouble(&ok);
                    if (ok) {
                        m_detailsProgressBar->setValue(static_cast<int>(percent));
                    }
                } else if (statusText == "Complete") {
                    m_detailsProgressBar->setValue(100);
                } else {
                    m_detailsProgressBar->setValue(0);
                }
            }
            
            QTableWidgetItem *speedItem = m_downloadsTable->item(currentRow, 4);
            if (speedItem) {
                m_detailsSpeedLabel->setText(speedItem->text());
            }
            
            QTableWidgetItem *timeItem = m_downloadsTable->item(currentRow, 3);
            if (timeItem) {
                m_detailsTimeLeftLabel->setText(timeItem->text());
            }
        }
    } else {
        // No selection
        m_detailsUrlLabel->setText("Select a download to view details");
        m_detailsStatusLabel->setText("Ready");
        m_detailsSizeLabel->setText("Unknown");
        m_detailsSpeedLabel->setText("0 MB/sec");
        m_detailsTimeLeftLabel->setText("Unknown");
        m_detailsProgressBar->setValue(0);
    }
}

void MainWindow::showNotification(const QString &title, const QString &message)
{
    if (m_trayIcon) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    }
}

QString MainWindow::formatFileSize(qint64 bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = bytes;
    
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    return QString("%1 %2").arg(size, 0, 'f', unitIndex > 0 ? 2 : 0).arg(units[unitIndex]);
}

QString MainWindow::formatSpeed(qint64 bytesPerSecond)
{
    return formatFileSize(bytesPerSecond) + "/sec";
}

QString MainWindow::formatTimeRemaining(qint64 seconds)
{
    if (seconds <= 0) return "0 sec";
    
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        return QString("%1h %2m %3s").arg(hours).arg(minutes).arg(secs);
    } else if (minutes > 0) {
        return QString("%1m %2s").arg(minutes).arg(secs);
    } else {
        return QString("%1 sec").arg(secs);
    }
}

QIcon MainWindow::getFileTypeIcon(const QString &filename)
{
    QFileInfo info(filename);
    QString extension = info.suffix().toLower();
    
    // For now, return empty icon since we removed icon resources
    // Icons can be added back later when resource system is properly set up
    return QIcon();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
        m_trayIcon->showMessage("LDM", "Application minimized to tray.", QSystemTrayIcon::Information, 2000);
    } else {
        event->accept();
    }
}