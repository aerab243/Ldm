#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QToolBar>
#include <QTreeWidget>
#include <QTableWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QStatusBar>
#include <QSplitter>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QMenuBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QTimer>
#include <QSettings>
#include <QTextEdit>
#include <QTabWidget>
#include <QCloseEvent>
#include <QStyle>
#include <iostream>

class LDMMainWindow : public QMainWindow
{
public:
    LDMMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM - Like Download Manager v1.0.0");
        setMinimumSize(900, 600);
        resize(1200, 800);

        setupMenuBar();
        setupToolBar();
        setupCentralWidget();
        setupStatusBar();

        // Load settings
        loadSettings();

        std::cout << "LDM GUI initialized successfully!" << std::endl;
    }

    ~LDMMainWindow()
    {
        saveSettings();
    }

private:
    void setupMenuBar()
    {
        QMenuBar *menuBar = this->menuBar();

        // File menu
        QMenu *fileMenu = menuBar->addMenu("&File");
        QAction *addUrlAction = fileMenu->addAction("&Add URL");
        addUrlAction->setShortcut(QKeySequence::New);
        connect(addUrlAction, &QAction::triggered, this, &LDMMainWindow::onAddUrl);

        fileMenu->addSeparator();
        QAction *exitAction = fileMenu->addAction("E&xit");
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);

        // Downloads menu
        QMenu *downloadsMenu = menuBar->addMenu("&Downloads");
        m_resumeAction = downloadsMenu->addAction("&Resume");
        m_pauseAction = downloadsMenu->addAction("&Pause");
        m_stopAction = downloadsMenu->addAction("&Stop");
        downloadsMenu->addSeparator();
        m_deleteAction = downloadsMenu->addAction("&Delete");

        // Connect download actions
        connect(m_resumeAction, &QAction::triggered, this, &LDMMainWindow::onResume);
        connect(m_pauseAction, &QAction::triggered, this, &LDMMainWindow::onPause);
        connect(m_stopAction, &QAction::triggered, this, &LDMMainWindow::onStop);
        connect(m_deleteAction, &QAction::triggered, this, &LDMMainWindow::onDelete);

        // Tools menu
        QMenu *toolsMenu = menuBar->addMenu("&Tools");
        QAction *settingsAction = toolsMenu->addAction("&Settings");
        connect(settingsAction, &QAction::triggered, this, &LDMMainWindow::onSettings);

        // Help menu
        QMenu *helpMenu = menuBar->addMenu("&Help");
        QAction *aboutAction = helpMenu->addAction("&About");
        connect(aboutAction, &QAction::triggered, this, &LDMMainWindow::onAbout);
    }

    void setupToolBar()
    {
        QToolBar *toolBar = addToolBar("Main Toolbar");
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        // Add URL button
        QAction *addUrlAction = toolBar->addAction("Add URL");
        addUrlAction->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
        connect(addUrlAction, &QAction::triggered, this, &LDMMainWindow::onAddUrl);

        toolBar->addSeparator();

        // Download control buttons
        m_resumeToolAction = toolBar->addAction("Resume");
        m_resumeToolAction->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        connect(m_resumeToolAction, &QAction::triggered, this, &LDMMainWindow::onResume);

        m_pauseToolAction = toolBar->addAction("Pause");
        m_pauseToolAction->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        connect(m_pauseToolAction, &QAction::triggered, this, &LDMMainWindow::onPause);

        m_stopToolAction = toolBar->addAction("Stop");
        m_stopToolAction->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        connect(m_stopToolAction, &QAction::triggered, this, &LDMMainWindow::onStop);

        toolBar->addSeparator();

        m_deleteToolAction = toolBar->addAction("Delete");
        m_deleteToolAction->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
        connect(m_deleteToolAction, &QAction::triggered, this, &LDMMainWindow::onDelete);

        toolBar->addSeparator();

        // Settings button
        QAction *settingsAction = toolBar->addAction("Settings");
        settingsAction->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
        connect(settingsAction, &QAction::triggered, this, &LDMMainWindow::onSettings);

        // Update action states
        updateActions();
    }

    void setupCentralWidget()
    {
        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

        // Left panel - Categories
        m_categoryTree = new QTreeWidget;
        m_categoryTree->setHeaderLabel("Categories");
        m_categoryTree->setMaximumWidth(250);
        m_categoryTree->setMinimumWidth(200);

        // Add category items
        QStringList categories = {
            "All Downloads", "Downloading", "Completed", "Unfinished",
            "Compressed", "Documents", "Music", "Videos", "Programs"
        };

        for (const QString &category : categories) {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_categoryTree);
            item->setText(0, category);
            if (category == "All Downloads") {
                item->setSelected(true);
            }
        }

        connect(m_categoryTree, &QTreeWidget::itemClicked, this, &LDMMainWindow::onCategoryChanged);

        // Right panel with splitter
        QSplitter *rightSplitter = new QSplitter(Qt::Vertical);

        // Downloads table
        m_downloadsTable = new QTableWidget(0, 7);
        QStringList headers = {"File Name", "Size", "Downloaded", "Progress", "Speed", "Time Left", "Status"};
        m_downloadsTable->setHorizontalHeaderLabels(headers);
        m_downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_downloadsTable->setAlternatingRowColors(true);
        m_downloadsTable->horizontalHeader()->setStretchLastSection(true);

        // Set column widths
        m_downloadsTable->setColumnWidth(0, 250); // File Name
        m_downloadsTable->setColumnWidth(1, 80);  // Size
        m_downloadsTable->setColumnWidth(2, 80);  // Downloaded
        m_downloadsTable->setColumnWidth(3, 100); // Progress
        m_downloadsTable->setColumnWidth(4, 80);  // Speed
        m_downloadsTable->setColumnWidth(5, 80);  // Time Left

        connect(m_downloadsTable, &QTableWidget::itemSelectionChanged, this, &LDMMainWindow::onSelectionChanged);

        // Details panel
        QGroupBox *detailsGroup = new QGroupBox("Download Details");
        QVBoxLayout *detailsLayout = new QVBoxLayout(detailsGroup);

        // File info
        QHBoxLayout *fileInfoLayout = new QHBoxLayout;
        fileInfoLayout->addWidget(new QLabel("File:"));
        m_fileNameLabel = new QLabel("No download selected");
        fileInfoLayout->addWidget(m_fileNameLabel);
        fileInfoLayout->addStretch();
        detailsLayout->addLayout(fileInfoLayout);

        // URL info
        QHBoxLayout *urlLayout = new QHBoxLayout;
        urlLayout->addWidget(new QLabel("URL:"));
        m_urlLabel = new QLabel("-");
        m_urlLabel->setWordWrap(true);
        urlLayout->addWidget(m_urlLabel);
        detailsLayout->addLayout(urlLayout);

        // Progress info
        QHBoxLayout *progressLayout = new QHBoxLayout;
        progressLayout->addWidget(new QLabel("Progress:"));
        m_detailsProgressBar = new QProgressBar;
        progressLayout->addWidget(m_detailsProgressBar);
        detailsLayout->addLayout(progressLayout);

        // Status info
        QGridLayout *statusGrid = new QGridLayout;
        statusGrid->addWidget(new QLabel("Status:"), 0, 0);
        m_statusLabel = new QLabel("Waiting");
        statusGrid->addWidget(m_statusLabel, 0, 1);

        statusGrid->addWidget(new QLabel("Speed:"), 1, 0);
        m_speedLabel = new QLabel("0 KB/s");
        statusGrid->addWidget(m_speedLabel, 1, 1);

        statusGrid->addWidget(new QLabel("Time Left:"), 2, 0);
        m_timeLeftLabel = new QLabel("-");
        statusGrid->addWidget(m_timeLeftLabel, 2, 1);

        detailsLayout->addLayout(statusGrid);
        detailsLayout->addStretch();

        rightSplitter->addWidget(m_downloadsTable);
        rightSplitter->addWidget(detailsGroup);
        rightSplitter->setSizes({500, 200});

        mainLayout->addWidget(m_categoryTree);
        mainLayout->addWidget(rightSplitter);
        mainLayout->setStretch(1, 1);

        // Add some sample data
        addSampleDownloads();
    }

    void setupStatusBar()
    {
        QStatusBar *status = statusBar();

        m_globalSpeedLabel = new QLabel("Speed: 0 KB/s");
        m_downloadsCountLabel = new QLabel("Downloads: 0");

        status->addWidget(m_globalSpeedLabel);
        status->addPermanentWidget(m_downloadsCountLabel);
        status->showMessage("Ready");
    }

    void addSampleDownloads()
    {
        // Add some demo downloads to show the interface
        addDownloadRow("Ubuntu-22.04-desktop-amd64.iso", "4.6 GB", "2.3 GB", 50, "1.2 MB/s", "28 min", "Downloading");
        addDownloadRow("VLC-media-player.exe", "42 MB", "42 MB", 100, "0 KB/s", "Complete", "Completed");
        addDownloadRow("sample-video.mp4", "156 MB", "78 MB", 50, "800 KB/s", "2 min", "Downloading");
        addDownloadRow("document.pdf", "2.1 MB", "0 MB", 0, "0 KB/s", "-", "Waiting");
    }

    void addDownloadRow(const QString &fileName, const QString &size, const QString &downloaded,
                       int progress, const QString &speed, const QString &timeLeft, const QString &status)
    {
        int row = m_downloadsTable->rowCount();
        m_downloadsTable->insertRow(row);

        m_downloadsTable->setItem(row, 0, new QTableWidgetItem(fileName));
        m_downloadsTable->setItem(row, 1, new QTableWidgetItem(size));
        m_downloadsTable->setItem(row, 2, new QTableWidgetItem(downloaded));

        QProgressBar *progressBar = new QProgressBar;
        progressBar->setValue(progress);
        progressBar->setFormat(QString("%1%").arg(progress));
        m_downloadsTable->setCellWidget(row, 3, progressBar);

        m_downloadsTable->setItem(row, 4, new QTableWidgetItem(speed));
        m_downloadsTable->setItem(row, 5, new QTableWidgetItem(timeLeft));
        m_downloadsTable->setItem(row, 6, new QTableWidgetItem(status));
    }

    void loadSettings()
    {
        QSettings settings;
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowState").toByteArray());
    }

    void saveSettings()
    {
        QSettings settings;
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
    }

    void updateActions()
    {
        bool hasSelection = m_downloadsTable->currentRow() >= 0;

        m_resumeAction->setEnabled(hasSelection);
        m_pauseAction->setEnabled(hasSelection);
        m_stopAction->setEnabled(hasSelection);
        m_deleteAction->setEnabled(hasSelection);

        m_resumeToolAction->setEnabled(hasSelection);
        m_pauseToolAction->setEnabled(hasSelection);
        m_stopToolAction->setEnabled(hasSelection);
        m_deleteToolAction->setEnabled(hasSelection);
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        saveSettings();
        QMainWindow::closeEvent(event);
    }

private slots:
    void onAddUrl()
    {
        bool ok;
        QString url = QInputDialog::getText(this, "Add Download URL",
                                          "Enter the URL to download:", QLineEdit::Normal, "", &ok);
        if (ok && !url.isEmpty()) {
            QString fileName = url.split('/').last();
            if (fileName.isEmpty()) fileName = "unknown_file";

            addDownloadRow(fileName, "Unknown", "0 MB", 0, "0 KB/s", "-", "Waiting");
            QMessageBox::information(this, "Download Added",
                                   QString("Added to download queue:\n%1").arg(url));
        }
    }

    void onResume()
    {
        int row = m_downloadsTable->currentRow();
        if (row >= 0) {
            m_downloadsTable->setItem(row, 6, new QTableWidgetItem("Downloading"));
            showMessage("Download resumed");
        }
    }

    void onPause()
    {
        int row = m_downloadsTable->currentRow();
        if (row >= 0) {
            m_downloadsTable->setItem(row, 6, new QTableWidgetItem("Paused"));
            showMessage("Download paused");
        }
    }

    void onStop()
    {
        int row = m_downloadsTable->currentRow();
        if (row >= 0) {
            m_downloadsTable->setItem(row, 6, new QTableWidgetItem("Stopped"));
            showMessage("Download stopped");
        }
    }

    void onDelete()
    {
        int row = m_downloadsTable->currentRow();
        if (row >= 0) {
            QString fileName = m_downloadsTable->item(row, 0)->text();
            int ret = QMessageBox::question(this, "Delete Download",
                                          QString("Delete download '%1'?").arg(fileName),
                                          QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::Yes) {
                m_downloadsTable->removeRow(row);
                showMessage("Download deleted");
            }
        }
    }

    void onSettings()
    {
        QMessageBox::information(this, "Settings",
                               "Settings dialog will be implemented in future versions.\n\n"
                               "Current features:\n"
                               "• Download management interface\n"
                               "• Category organization\n"
                               "• Progress tracking\n"
                               "• Modern Qt6 interface");
    }

    void onAbout()
    {
        QMessageBox::about(this, "About LDM",
            "<h3>LDM - Like Download Manager v1.0.0</h3>"
            "<p>A modern download manager for Linux with IDM-style interface.</p>"
            "<p><b>Developer:</b> Anna-el Gerard RABENANDRASANA (aerab243)<br>"
            "<b>Email:</b> aerabenandrasana@gmail.com<br>"
            "<b>Project:</b> <a href='https://github.com/aerab243/ldm'>https://github.com/aerab243/ldm</a></p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>Modern Qt6 interface</li>"
            "<li>Multi-threaded downloads</li>"
            "<li>Browser integration ready</li>"
            "<li>Category-based organization</li>"
            "<li>Progress tracking and statistics</li>"
            "</ul>");
    }

    void onCategoryChanged()
    {
        QTreeWidgetItem *current = m_categoryTree->currentItem();
        if (current) {
            QString category = current->text(0);
            showMessage(QString("Viewing category: %1").arg(category));
        }
    }

    void onSelectionChanged()
    {
        updateActions();

        int row = m_downloadsTable->currentRow();
        if (row >= 0) {
            QString fileName = m_downloadsTable->item(row, 0)->text();
            QString status = m_downloadsTable->item(row, 6)->text();
            QString speed = m_downloadsTable->item(row, 4)->text();
            QString timeLeft = m_downloadsTable->item(row, 5)->text();

            m_fileNameLabel->setText(fileName);
            m_urlLabel->setText(QString("https://example.com/downloads/%1").arg(fileName));
            m_statusLabel->setText(status);
            m_speedLabel->setText(speed);
            m_timeLeftLabel->setText(timeLeft);

            QProgressBar *progressBar = qobject_cast<QProgressBar*>(m_downloadsTable->cellWidget(row, 3));
            if (progressBar) {
                m_detailsProgressBar->setValue(progressBar->value());
            }
        } else {
            m_fileNameLabel->setText("No download selected");
            m_urlLabel->setText("-");
            m_statusLabel->setText("-");
            m_speedLabel->setText("0 KB/s");
            m_timeLeftLabel->setText("-");
            m_detailsProgressBar->setValue(0);
        }
    }

    void showMessage(const QString &message)
    {
        statusBar()->showMessage(message, 3000);
    }

private:
    // UI components
    QTreeWidget *m_categoryTree;
    QTableWidget *m_downloadsTable;
    QLabel *m_fileNameLabel;
    QLabel *m_urlLabel;
    QLabel *m_statusLabel;
    QLabel *m_speedLabel;
    QLabel *m_timeLeftLabel;
    QProgressBar *m_detailsProgressBar;
    QLabel *m_globalSpeedLabel;
    QLabel *m_downloadsCountLabel;

    // Actions
    QAction *m_resumeAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
    QAction *m_deleteAction;
    QAction *m_resumeToolAction;
    QAction *m_pauseToolAction;
    QAction *m_stopToolAction;
    QAction *m_deleteToolAction;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("LDM");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");
    app.setOrganizationDomain("github.com/aerab243");

    // Handle command line arguments
    if (argc > 1) {
        QString arg = QString(argv[1]);
        if (arg == "--version" || arg == "-v") {
            std::cout << "LDM - Like Download Manager v1.0.0" << std::endl;
            std::cout << "Developer: Anna-el Gerard RABENANDRASANA (aerab243)" << std::endl;
            std::cout << "Project: https://github.com/aerab243/ldm" << std::endl;
            std::cout << "GUI Version with Qt6" << std::endl;
            return 0;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "LDM - Like Download Manager v1.0.0" << std::endl;
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --version, -v    Show version information" << std::endl;
            std::cout << "  --help, -h       Show this help message" << std::endl;
            std::cout << "  --no-tray        Disable system tray (GUI has no tray by default)" << std::endl;
            return 0;
        }
    }

    LDMMainWindow window;
    window.show();

    return app.exec();
}
