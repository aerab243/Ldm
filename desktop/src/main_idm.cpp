#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QStandardPaths>
#include <QDir>
#include <QInputDialog>
#include <QLineEdit>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

class LDMMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LDMMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM - Like Download Manager 1.0.0");
        setMinimumSize(1000, 700);
        resize(1200, 800);
        
        setupUI();
        setupTrayIcon();
        setupConnections();
        
        // Fade in animation
        QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
        fadeIn->setDuration(300);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
        
        // Show ready status
        statusBar()->showMessage("Ready - IDM-style interface loaded");
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (m_trayIcon && m_trayIcon->isVisible()) {
            hide();
            event->ignore();
            if (m_trayIcon) {
                m_trayIcon->showMessage("LDM", "Application minimized to tray.", QSystemTrayIcon::Information, 2000);
            }
        } else {
            event->accept();
        }
    }

private slots:
    void onAddUrl()
    {
        QDialog dialog(this);
        dialog.setWindowTitle("Add URL - LDM");
        dialog.setFixedSize(500, 300);
        
        QFormLayout *layout = new QFormLayout(&dialog);
        
        // URL input
        QLineEdit *urlEdit = new QLineEdit();
        urlEdit->setPlaceholderText("Enter URL to download...");
        layout->addRow("URL:", urlEdit);
        
        // Save location
        QLineEdit *saveEdit = new QLineEdit();
        saveEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        layout->addRow("Save to:", saveEdit);
        
        // Category selection
        QComboBox *categoryCombo = new QComboBox();
        categoryCombo->addItems({"All Downloads", "Compressed", "Documents", "Music", "Video", "Programs"});
        layout->addRow("Category:", categoryCombo);
        
        // Number of connections
        QSpinBox *connectionsSpinBox = new QSpinBox();
        connectionsSpinBox->setRange(1, 8);
        connectionsSpinBox->setValue(4);
        layout->addRow("Connections:", connectionsSpinBox);
        
        // Start download immediately
        QCheckBox *startCheckBox = new QCheckBox("Start download immediately");
        startCheckBox->setChecked(true);
        layout->addRow(startCheckBox);
        
        // Buttons
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout->addRow(buttonBox);
        
        connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        
        if (dialog.exec() == QDialog::Accepted) {
            QString url = urlEdit->text().trimmed();
            if (!url.isEmpty()) {
                addDownloadToTable(url, categoryCombo->currentText(), saveEdit->text());
                statusBar()->showMessage("Download added: " + url);
            } else {
                QMessageBox::warning(this, "Add URL", "Please enter a valid URL.");
            }
        }
    }
    
    void onResume()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
            QTableWidgetItem *nameItem = m_downloadsTable->item(currentRow, 0);
            QString fileName = nameItem ? nameItem->text() : "Unknown";
            statusBar()->showMessage("Resuming: " + fileName);
        } else {
            QMessageBox::information(this, "Resume", "Please select a download to resume.");
        }
    }
    
    void onStop()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
            QTableWidgetItem *nameItem = m_downloadsTable->item(currentRow, 0);
            QString fileName = nameItem ? nameItem->text() : "Unknown";
            statusBar()->showMessage("Stopping: " + fileName);
        } else {
            QMessageBox::information(this, "Stop", "Please select a download to stop.");
        }
    }
    
    void onDelete()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
            m_downloadsTable->removeRow(currentRow);
            statusBar()->showMessage("Download deleted");
            updateDetailsPanel();
        } else {
            QMessageBox::information(this, "Delete", "Please select a download to delete.");
        }
    }
    
    void onOptions()
    {
        QMessageBox::information(this, "Options", 
            "Settings and options dialog will be integrated.\n\n"
            "This demonstrates the complete IDM-style interface structure.");
    }
    
    void onAbout()
    {
        QMessageBox::about(this, "About LDM", 
            "LDM - Like Download Manager\n"
            "Version 1.0.0\n\n"
            "A powerful download manager with IDM-style interface.\n\n"
            "Built with Qt 6 and C++20\n"
            "Interface redesigned to match Internet Download Manager");
    }
    
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::DoubleClick) {
            show();
            activateWindow();
        }
    }
    
    void onSelectionChanged()
    {
        updateDetailsPanel();
    }
    
    void onCategoryChanged()
    {
        QTreeWidgetItem *current = m_categoriesTree->currentItem();
        if (current) {
            QString category = current->text(0);
            statusBar()->showMessage("Showing: " + category);
        }
    }

private:
    QTreeWidget *m_categoriesTree;
    QTableWidget *m_downloadsTable;
    QLabel *m_detailsUrlLabel;
    QLabel *m_detailsStatusLabel;
    QLabel *m_detailsSizeLabel;
    QLabel *m_detailsSpeedLabel;
    QLabel *m_detailsTimeLeftLabel;
    QProgressBar *m_detailsProgressBar;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    
    void addDownloadToTable(const QString &url, const QString &category, const QString &savePath);
    
    void setupUI()
    {
        setupMenuBar();
        setupToolBar();
        setupMainLayout();
        setupStatusBar();
        applyIDMStyling();
    }
    
    void setupMenuBar()
    {
        QMenuBar *menuBar = this->menuBar();
        
        QMenu *fileMenu = menuBar->addMenu("&File");
        fileMenu->addAction("&Add URL...", this, &LDMMainWindow::onAddUrl, QKeySequence::New);
        fileMenu->addSeparator();
        fileMenu->addAction("&Exit", this, &QWidget::close, QKeySequence::Quit);
        
        QMenu *downloadsMenu = menuBar->addMenu("&Downloads");
        downloadsMenu->addAction("&Resume", this, &LDMMainWindow::onResume);
        QAction *pauseAction = downloadsMenu->addAction("&Pause");
        Q_UNUSED(pauseAction)
        downloadsMenu->addAction("&Stop", this, &LDMMainWindow::onStop);
        downloadsMenu->addAction("&Delete", this, &LDMMainWindow::onDelete);
        
        QMenu *viewMenu = menuBar->addMenu("&View");
        viewMenu->addAction("&Toolbar", [this]() { 
            QToolBar *toolbar = this->findChild<QToolBar*>();
            if (toolbar) toolbar->setVisible(!toolbar->isVisible()); 
        });
        
        QMenu *toolsMenu = menuBar->addMenu("&Tools");
        toolsMenu->addAction("&Options", this, &LDMMainWindow::onOptions);
        
        QMenu *helpMenu = menuBar->addMenu("&Help");
        helpMenu->addAction("&About", this, &LDMMainWindow::onAbout);
    }
    
    void setupToolBar()
    {
        QToolBar *toolbar = addToolBar("Main");
        toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        
        toolbar->addAction("Add URL", this, &LDMMainWindow::onAddUrl);
        toolbar->addAction("Resume", this, &LDMMainWindow::onResume);
        toolbar->addAction("Stop", this, &LDMMainWindow::onStop);
        toolbar->addAction("Stop All", [this]() { statusBar()->showMessage("Stop All clicked"); });
        toolbar->addAction("Delete", this, &LDMMainWindow::onDelete);
        toolbar->addAction("Delete C", [this]() { statusBar()->showMessage("Delete Completed clicked"); });
        toolbar->addSeparator();
        toolbar->addAction("Options", this, &LDMMainWindow::onOptions);
        toolbar->addAction("Scheduler", [this]() { statusBar()->showMessage("Scheduler clicked"); });
        toolbar->addAction("Start Qu", [this]() { statusBar()->showMessage("Start Queue clicked"); });
        toolbar->addAction("Stop Qu", [this]() { statusBar()->showMessage("Stop Queue clicked"); });
        toolbar->addAction("Grabber", [this]() { statusBar()->showMessage("Grabber clicked"); });
        toolbar->addAction("Tell a Frie", [this]() { statusBar()->showMessage("Tell a Friend clicked"); });
    }
    
    void setupMainLayout()
    {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setSpacing(5);
        mainLayout->setContentsMargins(5, 5, 5, 5);
        
        QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
        
        // Left panel - Categories
        QGroupBox *categoriesGroup = new QGroupBox("Categories");
        categoriesGroup->setFixedWidth(200);
        QVBoxLayout *categoriesLayout = new QVBoxLayout(categoriesGroup);
        
        m_categoriesTree = new QTreeWidget();
        m_categoriesTree->setHeaderHidden(true);
        m_categoriesTree->setRootIsDecorated(false);
        
        QTreeWidgetItem *allDownloads = new QTreeWidgetItem(m_categoriesTree, QStringList("All Downloads"));
        QTreeWidgetItem *compressed = new QTreeWidgetItem(m_categoriesTree, QStringList("Compressed"));
        QTreeWidgetItem *documents = new QTreeWidgetItem(m_categoriesTree, QStringList("Documents"));
        QTreeWidgetItem *music = new QTreeWidgetItem(m_categoriesTree, QStringList("Music"));
        QTreeWidgetItem *video = new QTreeWidgetItem(m_categoriesTree, QStringList("Video"));
        QTreeWidgetItem *programs = new QTreeWidgetItem(m_categoriesTree, QStringList("Programs"));
        QTreeWidgetItem *unfinished = new QTreeWidgetItem(m_categoriesTree, QStringList("Unfinished"));
        QTreeWidgetItem *finished = new QTreeWidgetItem(m_categoriesTree, QStringList("Finished"));
        QTreeWidgetItem *queues = new QTreeWidgetItem(m_categoriesTree, QStringList("Queues"));
        
        allDownloads->setSelected(true);
        
        categoriesLayout->addWidget(m_categoriesTree);
        
        // Right panel - Downloads and details
        QWidget *rightPanel = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setContentsMargins(0, 0, 0, 0);
        
        // Downloads table
        QGroupBox *downloadsGroup = new QGroupBox("Downloads");
        QVBoxLayout *downloadsLayout = new QVBoxLayout(downloadsGroup);
        
        m_downloadsTable = new QTableWidget();
        m_downloadsTable->setColumnCount(7);
        QStringList headers = {"File Name", "Size", "Status", "Time left", "Transfer rate", "Last Try", "Description"};
        m_downloadsTable->setHorizontalHeaderLabels(headers);
        m_downloadsTable->setAlternatingRowColors(true);
        m_downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_downloadsTable->horizontalHeader()->setStretchLastSection(true);
        m_downloadsTable->verticalHeader()->setVisible(false);
        
        // Add sample data
        m_downloadsTable->insertRow(0);
        m_downloadsTable->setItem(0, 0, new QTableWidgetItem("undercover.mp3"));
        m_downloadsTable->setItem(0, 1, new QTableWidgetItem("7.16 MB"));
        m_downloadsTable->setItem(0, 2, new QTableWidgetItem("Complete"));
        m_downloadsTable->setItem(0, 3, new QTableWidgetItem(""));
        m_downloadsTable->setItem(0, 4, new QTableWidgetItem(""));
        m_downloadsTable->setItem(0, 5, new QTableWidgetItem("Nov 08 ..."));
        m_downloadsTable->setItem(0, 6, new QTableWidgetItem(""));
        
        m_downloadsTable->insertRow(1);
        m_downloadsTable->setItem(1, 0, new QTableWidgetItem("mult.flv"));
        m_downloadsTable->setItem(1, 1, new QTableWidgetItem("37.017 MB"));
        m_downloadsTable->setItem(1, 2, new QTableWidgetItem("94.97%"));
        m_downloadsTable->setItem(1, 3, new QTableWidgetItem("0 sec"));
        m_downloadsTable->setItem(1, 4, new QTableWidgetItem("4.67 MB/sec"));
        m_downloadsTable->setItem(1, 5, new QTableWidgetItem("Oct 13 2..."));
        m_downloadsTable->setItem(1, 6, new QTableWidgetItem(""));
        
        downloadsLayout->addWidget(m_downloadsTable);
        
        // Download details panel
        QGroupBox *detailsGroup = new QGroupBox("Download status");
        detailsGroup->setFixedHeight(150);
        QGridLayout *detailsLayout = new QGridLayout(detailsGroup);
        
        detailsLayout->addWidget(new QLabel("URL:"), 0, 0);
        m_detailsUrlLabel = new QLabel("Select a download to view details");
        detailsLayout->addWidget(m_detailsUrlLabel, 0, 1, 1, 2);
        
        detailsLayout->addWidget(new QLabel("Status:"), 1, 0);
        m_detailsStatusLabel = new QLabel("Ready");
        detailsLayout->addWidget(m_detailsStatusLabel, 1, 1);
        
        detailsLayout->addWidget(new QLabel("File size:"), 2, 0);
        m_detailsSizeLabel = new QLabel("Unknown");
        detailsLayout->addWidget(m_detailsSizeLabel, 2, 1);
        
        detailsLayout->addWidget(new QLabel("Downloaded:"), 2, 2);
        QLabel *downloadedLabel = new QLabel("0 MB (0.00 %)");
        detailsLayout->addWidget(downloadedLabel, 2, 3);
        
        detailsLayout->addWidget(new QLabel("Transfer rate:"), 3, 0);
        m_detailsSpeedLabel = new QLabel("0 MB/sec");
        detailsLayout->addWidget(m_detailsSpeedLabel, 3, 1);
        
        detailsLayout->addWidget(new QLabel("Time left:"), 3, 2);
        m_detailsTimeLeftLabel = new QLabel("Unknown");
        detailsLayout->addWidget(m_detailsTimeLeftLabel, 3, 3);
        
        detailsLayout->addWidget(new QLabel("Resume capability:"), 4, 0);
        QLabel *resumeLabel = new QLabel("Yes");
        detailsLayout->addWidget(resumeLabel, 4, 1);
        
        m_detailsProgressBar = new QProgressBar();
        m_detailsProgressBar->setMinimum(0);
        m_detailsProgressBar->setMaximum(100);
        m_detailsProgressBar->setValue(0);
        detailsLayout->addWidget(m_detailsProgressBar, 5, 0, 1, 4);
        
        QHBoxLayout *detailsButtonsLayout = new QHBoxLayout();
        QPushButton *showDetailsBtn = new QPushButton("Show details >>");
        QPushButton *pauseBtn = new QPushButton("Pause");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        detailsButtonsLayout->addWidget(showDetailsBtn);
        detailsButtonsLayout->addStretch();
        detailsButtonsLayout->addWidget(pauseBtn);
        detailsButtonsLayout->addWidget(cancelBtn);
        detailsLayout->addLayout(detailsButtonsLayout, 6, 0, 1, 4);
        
        rightLayout->addWidget(downloadsGroup);
        rightLayout->addWidget(detailsGroup);
        
        mainSplitter->addWidget(categoriesGroup);
        mainSplitter->addWidget(rightPanel);
        mainSplitter->setSizes({200, 800});
        
        mainLayout->addWidget(mainSplitter);
    }
    
    void setupStatusBar()
    {
        QStatusBar *status = statusBar();
        status->showMessage("Ready - IDM-style interface loaded");
        
        QLabel *progressLabel = new QLabel("4.67 MB/sec");
        QProgressBar *overallProgress = new QProgressBar();
        overallProgress->setMaximum(100);
        overallProgress->setValue(95);
        overallProgress->setMaximumWidth(100);
        
        status->addPermanentWidget(progressLabel);
        status->addPermanentWidget(overallProgress);
    }
    
    void setupTrayIcon()
    {
        m_trayIcon = new QSystemTrayIcon(this);
        m_trayIcon->setIcon(QIcon(":/icons/ldm.png"));
        
        m_trayMenu = new QMenu(this);
        m_trayMenu->addAction("Show", [this]() { show(); activateWindow(); });
        m_trayMenu->addAction("Add URL", this, &LDMMainWindow::onAddUrl);
        m_trayMenu->addSeparator();
        m_trayMenu->addAction("Exit", this, &QWidget::close);
        
        m_trayIcon->setContextMenu(m_trayMenu);
        m_trayIcon->show();
        
        connect(m_trayIcon, &QSystemTrayIcon::activated,
                this, &LDMMainWindow::onTrayIconActivated);
    }
    
    void setupConnections()
    {
        connect(m_categoriesTree, &QTreeWidget::currentItemChanged,
                this, &LDMMainWindow::onCategoryChanged);
        connect(m_downloadsTable, &QTableWidget::currentCellChanged,
                this, &LDMMainWindow::onSelectionChanged);
    }
    
    void updateDetailsPanel()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
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
            m_detailsUrlLabel->setText("Select a download to view details");
            m_detailsStatusLabel->setText("Ready");
            m_detailsSizeLabel->setText("Unknown");
            m_detailsSpeedLabel->setText("0 MB/sec");
            m_detailsTimeLeftLabel->setText("Unknown");
            m_detailsProgressBar->setValue(0);
        }
    }
    
    void applyIDMStyling()
    {
        setStyleSheet(
            "QMainWindow { background-color: #f0f0f0; }"
            "QToolBar { background-color: #e8e8e8; border: 1px solid #ccc; spacing: 2px; padding: 2px; }"
            "QTreeWidget { background-color: white; border: 1px solid #ccc; alternate-background-color: #f5f5f5; }"
            "QTableWidget { background-color: white; border: 1px solid #ccc; gridline-color: #ddd; alternate-background-color: #f9f9f9; }"
            "QGroupBox { font-weight: bold; border: 2px solid #ccc; border-radius: 5px; margin-top: 1ex; background-color: #fafafa; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
            "QStatusBar { background-color: #e8e8e8; border-top: 1px solid #ccc; }"
            "QProgressBar { border: 1px solid #ccc; border-radius: 3px; text-align: center; background-color: #f0f0f0; }"
            "QProgressBar::chunk { background-color: #4CAF50; border-radius: 2px; }"
        );
    }
};

void LDMMainWindow::addDownloadToTable(const QString &url, const QString &category, const QString &savePath)
{
    Q_UNUSED(category)
    Q_UNUSED(savePath)
    
    int row = m_downloadsTable->rowCount();
    m_downloadsTable->insertRow(row);
    
    // Extract filename from URL
    QString fileName = url.split('/').last();
    if (fileName.isEmpty()) {
        fileName = "download_" + QString::number(row + 1);
    }
    
    // Add items to table
    m_downloadsTable->setItem(row, 0, new QTableWidgetItem(fileName));
    m_downloadsTable->setItem(row, 1, new QTableWidgetItem("Unknown"));
    m_downloadsTable->setItem(row, 2, new QTableWidgetItem("Ready"));
    m_downloadsTable->setItem(row, 3, new QTableWidgetItem("0 sec"));
    m_downloadsTable->setItem(row, 4, new QTableWidgetItem("0 MB/s"));
    m_downloadsTable->setItem(row, 5, new QTableWidgetItem("Ready"));
    m_downloadsTable->setItem(row, 6, new QTableWidgetItem(url));
    
    // Select the new row
    m_downloadsTable->selectRow(row);
    updateDetailsPanel();
}

#include "main_idm.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("LDM");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");
    app.setApplicationDisplayName("LDM - Like Download Manager");
    
    // Ensure data directory exists
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    LDMMainWindow window;
    window.show();

    return app.exec();
}