#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QMessageBox>

class SimpleMainWindow : public QMainWindow
{
public:
    SimpleMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM - Linux Download Manager 1.0.0");
        setMinimumSize(900, 600);
        resize(1200, 800);
        
        setupUI();
        setupMenuBar();
        setupToolBar();
        setupData();
        applyIDMStyling();
    }

private:
    QTreeWidget *m_categoriesTree;
    QTableWidget *m_downloadsTable;
    QGroupBox *m_detailsGroup;

    void setupUI()
    {
        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        
        QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
        mainLayout->addWidget(mainSplitter);
        
        // Left panel for categories
        QWidget *leftPanel = new QWidget;
        leftPanel->setFixedWidth(200);
        leftPanel->setStyleSheet("QWidget { background-color: #f8f9fa; border-right: 1px solid #dee2e6; }");
        
        QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setContentsMargins(5, 5, 5, 5);
        
        m_categoriesTree = new QTreeWidget;
        m_categoriesTree->setHeaderLabel("Categories");
        m_categoriesTree->setAlternatingRowColors(true);
        leftLayout->addWidget(m_categoriesTree);
        
        // Right panel for downloads table and details
        QWidget *rightPanel = new QWidget;
        QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setContentsMargins(5, 5, 5, 5);
        rightLayout->setSpacing(5);
        
        // Downloads table
        m_downloadsTable = new QTableWidget;
        m_downloadsTable->setColumnCount(7);
        
        QStringList headers;
        headers << "FILE NAME" << "SIZE" << "STATUS" << "TIME LEFT" << "SPEED" << "DATE" << "TYPE";
        m_downloadsTable->setHorizontalHeaderLabels(headers);
        
        m_downloadsTable->setAlternatingRowColors(true);
        m_downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_downloadsTable->setSortingEnabled(true);
        m_downloadsTable->verticalHeader()->setVisible(false);
        
        QHeaderView *header = m_downloadsTable->horizontalHeader();
        header->setStretchLastSection(true);
        header->resizeSection(0, 300);
        header->resizeSection(1, 80);
        header->resizeSection(2, 100);
        header->resizeSection(3, 80);
        header->resizeSection(4, 100);
        header->resizeSection(5, 120);
        header->resizeSection(6, 80);
        
        rightLayout->addWidget(m_downloadsTable, 1);
        
        // Details panel
        m_detailsGroup = new QGroupBox("Download status");
        m_detailsGroup->setFixedHeight(160);
        
        QHBoxLayout *detailsLayout = new QHBoxLayout(m_detailsGroup);
        
        // Left side - Info labels
        QVBoxLayout *leftDetailsLayout = new QVBoxLayout;
        
        QLabel *urlLabel = new QLabel("URL: Select a download to view details");
        urlLabel->setStyleSheet("font-weight: bold; color: #495057;");
        leftDetailsLayout->addWidget(urlLabel);
        
        QLabel *statusLabel = new QLabel("Status: Ready");
        statusLabel->setStyleSheet("color: #6c757d;");
        leftDetailsLayout->addWidget(statusLabel);
        
        QLabel *fileSizeLabel = new QLabel("File size: Unknown");
        fileSizeLabel->setStyleSheet("color: #6c757d;");
        leftDetailsLayout->addWidget(fileSizeLabel);
        
        QLabel *transferRateLabel = new QLabel("Transfer rate: 0 MB/sec");
        transferRateLabel->setStyleSheet("color: #6c757d;");
        leftDetailsLayout->addWidget(transferRateLabel);
        
        QLabel *resumeCapabilityLabel = new QLabel("Resume capability: Yes");
        resumeCapabilityLabel->setStyleSheet("color: #6c757d;");
        leftDetailsLayout->addWidget(resumeCapabilityLabel);
        
        leftDetailsLayout->addStretch();
        
        // Right side - Download info
        QVBoxLayout *rightDetailsLayout = new QVBoxLayout;
        
        QLabel *downloadedLabel = new QLabel("Downloaded: 0 MB (0.00 %)");
        downloadedLabel->setStyleSheet("color: #6c757d;");
        rightDetailsLayout->addWidget(downloadedLabel);
        
        QLabel *timeLeftLabel = new QLabel("Time left: Unknown");
        timeLeftLabel->setStyleSheet("color: #6c757d;");
        rightDetailsLayout->addWidget(timeLeftLabel);
        
        rightDetailsLayout->addStretch();
        
        // Bottom buttons
        QHBoxLayout *buttonsLayout = new QHBoxLayout;
        buttonsLayout->addStretch();
        
        QPushButton *pauseButton = new QPushButton("Pause");
        pauseButton->setFixedSize(80, 30);
        
        QPushButton *cancelButton = new QPushButton("Cancel");
        cancelButton->setFixedSize(80, 30);
        
        buttonsLayout->addWidget(pauseButton);
        buttonsLayout->addWidget(cancelButton);
        
        rightDetailsLayout->addLayout(buttonsLayout);
        
        detailsLayout->addLayout(leftDetailsLayout, 1);
        detailsLayout->addLayout(rightDetailsLayout, 1);
        
        rightLayout->addWidget(m_detailsGroup);
        
        mainSplitter->addWidget(leftPanel);
        mainSplitter->addWidget(rightPanel);
        mainSplitter->setStretchFactor(0, 0);
        mainSplitter->setStretchFactor(1, 1);
        
        // Status bar
        statusBar()->showMessage("Ready");
    }

    void setupMenuBar()
    {
        QMenuBar *menuBar = this->menuBar();
        
        // Tasks menu
        QMenu *tasksMenu = menuBar->addMenu("&Tasks");
        tasksMenu->addAction("&Add URL...");
        tasksMenu->addSeparator();
        tasksMenu->addAction("E&xit");
        
        // Downloads menu
        QMenu *downloadsMenu = menuBar->addMenu("&Downloads");
        downloadsMenu->addAction("&Resume");
        downloadsMenu->addAction("&Stop");
        downloadsMenu->addSeparator();
        downloadsMenu->addAction("&Delete");
        
        // View menu
        QMenu *viewMenu = menuBar->addMenu("&View");
        viewMenu->addAction("&Toolbar");
        viewMenu->addAction("&Status Bar");
        
        // Tools menu
        QMenu *toolsMenu = menuBar->addMenu("T&ools");
        toolsMenu->addAction("&Options...");
        
        // Help menu
        QMenu *helpMenu = menuBar->addMenu("&Help");
        helpMenu->addAction("&About LDM");
    }

    void setupToolBar()
    {
        QToolBar *toolBar = addToolBar("Main");
        toolBar->setMovable(false);
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        toolBar->setIconSize(QSize(24, 24));
        
        QAction *addUrlAction = toolBar->addAction("Add URL");
        addUrlAction->setToolTip("Add new download URL (Ctrl+N)");
        
        QAction *resumeAction = toolBar->addAction("Resume");
        resumeAction->setToolTip("Resume selected downloads");
        
        QAction *stopAction = toolBar->addAction("Stop");
        stopAction->setToolTip("Stop selected downloads");
        
        QAction *deleteAction = toolBar->addAction("Delete");
        deleteAction->setToolTip("Delete selected downloads");
        
        toolBar->addSeparator();
        
        QAction *optionsAction = toolBar->addAction("Options");
        optionsAction->setToolTip("Open settings");
    }

    void setupData()
    {
        // Populate categories with neutral styling
        m_categoriesTree->clear();
        
        QTreeWidgetItem *allItem = new QTreeWidgetItem(m_categoriesTree);
        allItem->setText(0, "All Downloads");
        
        QTreeWidgetItem *videoItem = new QTreeWidgetItem(m_categoriesTree);
        videoItem->setText(0, "Video");
        
        QTreeWidgetItem *audioItem = new QTreeWidgetItem(m_categoriesTree);
        audioItem->setText(0, "Audio");
        
        QTreeWidgetItem *documentsItem = new QTreeWidgetItem(m_categoriesTree);
        documentsItem->setText(0, "Documents");
        
        QTreeWidgetItem *archivesItem = new QTreeWidgetItem(m_categoriesTree);
        archivesItem->setText(0, "Archives");
        
        QTreeWidgetItem *programsItem = new QTreeWidgetItem(m_categoriesTree);
        programsItem->setText(0, "Programs");
        
        // Add separator
        QTreeWidgetItem *separator = new QTreeWidgetItem(m_categoriesTree);
        separator->setText(0, "---");
        separator->setDisabled(true);
        
        QTreeWidgetItem *completedItem = new QTreeWidgetItem(m_categoriesTree);
        completedItem->setText(0, "Completed");
        
        QTreeWidgetItem *inProgressItem = new QTreeWidgetItem(m_categoriesTree);
        inProgressItem->setText(0, "In Progress");
        
        QTreeWidgetItem *queuedItem = new QTreeWidgetItem(m_categoriesTree);
        queuedItem->setText(0, "Queued");
        
        m_categoriesTree->expandAll();
        allItem->setSelected(true);
        
        // Populate downloads table with realistic data
        struct SampleDownload {
            QString filename;
            QString size;
            QString status;
            QString timeLeft;
            QString speed;
            QString date;
            QString type;
        };

        QList<SampleDownload> samples = {
            {"Ubuntu_22.04.3_Desktop.iso", "4.7 GB", "Completed", "—", "—", "Today 14:32", "Archive"},
            {"Big_Buck_Bunny_4K.mp4", "1.2 GB", "68%", "2m 14s", "12.8 MB/s", "Today 14:45", "Video"},
            {"Linux_Administration_Guide.pdf", "15.7 MB", "Completed", "—", "—", "Yesterday", "Document"},
            {"nodejs-v20.10.0-linux-x64.tar.gz", "23.1 MB", "Paused", "—", "—", "Today 13:20", "Archive"},
            {"VLC_Media_Player_Setup.exe", "45.2 MB", "In Progress", "1m 32s", "8.4 MB/s", "Today 15:10", "Program"}
        };

        m_downloadsTable->setRowCount(samples.size());

        for (int i = 0; i < samples.size(); ++i) {
            const SampleDownload &download = samples[i];
            
            m_downloadsTable->setItem(i, 0, new QTableWidgetItem(download.filename));
            m_downloadsTable->setItem(i, 1, new QTableWidgetItem(download.size));
            m_downloadsTable->setItem(i, 2, new QTableWidgetItem(download.status));
            m_downloadsTable->setItem(i, 3, new QTableWidgetItem(download.timeLeft));
            m_downloadsTable->setItem(i, 4, new QTableWidgetItem(download.speed));
            m_downloadsTable->setItem(i, 5, new QTableWidgetItem(download.date));
            m_downloadsTable->setItem(i, 6, new QTableWidgetItem(download.type));
        }
    }

    void applyIDMStyling()
    {
        setStyleSheet(R"(
            QMainWindow {
                background-color: #ffffff;
            }
            
            QMenuBar {
                background-color: #ffffff;
                border-bottom: 1px solid #dee2e6;
                font-size: 14px;
                padding: 2px;
            }
            
            QMenuBar::item {
                padding: 6px 12px;
                background-color: transparent;
                color: #495057;
            }
            
            QMenuBar::item:selected {
                background-color: #e9ecef;
                border-radius: 4px;
            }
            
            QToolBar {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                           stop:0 #f8f9fa, stop:1 #e9ecef);
                border-bottom: 1px solid #dee2e6;
                spacing: 8px;
                padding: 8px;
            }
            
            QToolBar QToolButton {
                background-color: transparent;
                border: 1px solid transparent;
                border-radius: 4px;
                padding: 8px 12px;
                margin: 2px;
                color: #495057;
                font-size: 13px;
            }
            
            QToolBar QToolButton:hover {
                background-color: #e9ecef;
                border: 1px solid #adb5bd;
            }
            
            QToolBar QToolButton:pressed {
                background-color: #dee2e6;
                border: 1px solid #6c757d;
            }
            
            QTreeWidget {
                background-color: #f8f9fa;
                border: 1px solid #dee2e6;
                selection-background-color: #007bff;
                alternate-background-color: #ffffff;
                color: #495057;
                font-size: 14px;
            }
            
            QTreeWidget::item {
                padding: 8px;
                border-bottom: 1px solid #f1f3f4;
            }
            
            QTreeWidget::item:selected {
                background-color: #007bff;
                color: white;
            }
            
            QTreeWidget::item:hover {
                background-color: #e9ecef;
            }
            
            QTableWidget {
                background-color: #ffffff;
                border: 1px solid #dee2e6;
                gridline-color: #f1f3f4;
                selection-background-color: #007bff;
                alternate-background-color: #f8f9fa;
                color: #495057;
                font-size: 13px;
            }
            
            QTableWidget::item {
                padding: 8px;
                border-bottom: 1px solid #f1f3f4;
            }
            
            QTableWidget::item:selected {
                background-color: #007bff;
                color: white;
            }
            
            QHeaderView::section {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                           stop:0 #f8f9fa, stop:1 #e9ecef);
                border: 1px solid #dee2e6;
                padding: 10px;
                font-weight: bold;
                color: #495057;
                font-size: 12px;
            }
            
            QGroupBox {
                font-weight: bold;
                border: 1px solid #dee2e6;
                border-radius: 6px;
                margin: 8px 0;
                padding-top: 16px;
                background-color: #f8f9fa;
                color: #495057;
                font-size: 14px;
            }
            
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 12px;
                padding: 0 8px;
                background-color: #f8f9fa;
            }
            
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                           stop:0 #ffffff, stop:1 #f8f9fa);
                border: 1px solid #dee2e6;
                border-radius: 4px;
                padding: 8px 16px;
                color: #495057;
                font-weight: 500;
                font-size: 13px;
            }
            
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                           stop:0 #f8f9fa, stop:1 #e9ecef);
                border: 1px solid #adb5bd;
            }
            
            QPushButton:pressed {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                           stop:0 #e9ecef, stop:1 #dee2e6);
                border: 1px solid #6c757d;
            }
            
            QStatusBar {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                           stop:0 #f8f9fa, stop:1 #e9ecef);
                border-top: 1px solid #dee2e6;
                color: #6c757d;
                font-size: 13px;
                padding: 4px;
            }
            
            QSplitter::handle {
                background-color: #dee2e6;
            }
            
            QSplitter::handle:horizontal {
                width: 2px;
            }
        )");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SimpleMainWindow window;
    window.show();
    
    return app.exec();
}