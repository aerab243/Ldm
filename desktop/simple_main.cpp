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

class SimpleMainWindow : public QMainWindow
{
public:
    SimpleMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM - Like Download Manager 1.0.0");
        setMinimumSize(1000, 700);
        resize(1200, 800);

        setupUI();
    }

private slots:
    void onAddUrl() {
        QMessageBox::information(this, "Add URL", "Add URL functionality would be implemented here.");
    }
    
    void onAbout() {
        QMessageBox::about(this, "About LDM", 
            "LDM - Like Download Manager\n"
            "Version 1.0.0\n\n"
            "A powerful download manager inspired by IDM.\n\n"
            "Built with Qt 6 and C++20");
    }

private:
    void setupUI()
    {
        // Menu bar
        QMenuBar *menuBar = this->menuBar();
        QMenu *tasksMenu = menuBar->addMenu("&Tasks");
        QAction *addUrlAction = tasksMenu->addAction("&Add URL...");
        connect(addUrlAction, &QAction::triggered, this, &SimpleMainWindow::onAddUrl);
        
        QMenu *helpMenu = menuBar->addMenu("&Help");
        QAction *aboutAction = helpMenu->addAction("&About LDM");
        connect(aboutAction, &QAction::triggered, this, &SimpleMainWindow::onAbout);

        // Toolbar
        QToolBar *toolbar = addToolBar("Main");
        toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        
        toolbar->addAction("Add URL", this, &SimpleMainWindow::onAddUrl);
        toolbar->addAction("Resume");
        toolbar->addAction("Stop");
        toolbar->addAction("Stop All");
        toolbar->addAction("Delete");
        toolbar->addSeparator();
        toolbar->addAction("Options");
        toolbar->addAction("Scheduler");

        // Create central widget with splitters
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(2, 2, 2, 2);
        mainLayout->setSpacing(2);

        // Create horizontal splitter
        QSplitter *horizontalSplitter = new QSplitter(Qt::Horizontal, this);
        
        // Create vertical splitter
        QSplitter *verticalSplitter = new QSplitter(Qt::Vertical, this);

        // Categories tree
        QTreeWidget *categoriesTree = new QTreeWidget(this);
        categoriesTree->setHeaderLabel("Categories");
        categoriesTree->setMaximumWidth(250);
        categoriesTree->setMinimumWidth(180);
        
        // Add sample categories
        QTreeWidgetItem *allDownloads = new QTreeWidgetItem(categoriesTree);
        allDownloads->setText(0, "All Downloads");
        
        QTreeWidgetItem *compressed = new QTreeWidgetItem(categoriesTree);
        compressed->setText(0, "Compressed");
        
        QTreeWidgetItem *documents = new QTreeWidgetItem(categoriesTree);
        documents->setText(0, "Documents");
        
        QTreeWidgetItem *music = new QTreeWidgetItem(categoriesTree);
        music->setText(0, "Music");
        
        QTreeWidgetItem *programs = new QTreeWidgetItem(categoriesTree);
        programs->setText(0, "Programs");
        
        QTreeWidgetItem *video = new QTreeWidgetItem(categoriesTree);
        video->setText(0, "Video");
        
        QTreeWidgetItem *unfinished = new QTreeWidgetItem(categoriesTree);
        unfinished->setText(0, "Unfinished");
        
        QTreeWidgetItem *finished = new QTreeWidgetItem(categoriesTree);
        finished->setText(0, "Finished");
        
        QTreeWidgetItem *queues = new QTreeWidgetItem(categoriesTree);
        queues->setText(0, "Queues");
        
        categoriesTree->setCurrentItem(allDownloads);

        // Downloads table
        QTableWidget *downloadsTable = new QTableWidget(this);
        
        QStringList headers;
        headers << "File Name" << "Size" << "Status" << "Time left" << "Transfer rate" << "Last Try" << "Description";
        downloadsTable->setColumnCount(headers.size());
        downloadsTable->setHorizontalHeaderLabels(headers);
        
        downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        downloadsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
        downloadsTable->setAlternatingRowColors(true);
        downloadsTable->setSortingEnabled(true);
        downloadsTable->verticalHeader()->setVisible(false);
        
        // Set column widths
        QHeaderView *header = downloadsTable->horizontalHeader();
        header->setStretchLastSection(true);
        downloadsTable->setColumnWidth(0, 200); // File Name
        downloadsTable->setColumnWidth(1, 80);  // Size
        downloadsTable->setColumnWidth(2, 100); // Status
        downloadsTable->setColumnWidth(3, 80);  // Time left
        downloadsTable->setColumnWidth(4, 100); // Transfer rate
        downloadsTable->setColumnWidth(5, 120); // Last Try
        
        // Add sample data
        downloadsTable->insertRow(0);
        downloadsTable->setItem(0, 0, new QTableWidgetItem("undercover.mp3"));
        downloadsTable->setItem(0, 1, new QTableWidgetItem("7.16 MB"));
        downloadsTable->setItem(0, 2, new QTableWidgetItem("Complete"));
        downloadsTable->setItem(0, 3, new QTableWidgetItem(""));
        downloadsTable->setItem(0, 4, new QTableWidgetItem(""));
        downloadsTable->setItem(0, 5, new QTableWidgetItem("Nov 08 ..."));
        downloadsTable->setItem(0, 6, new QTableWidgetItem(""));
        
        downloadsTable->insertRow(1);
        downloadsTable->setItem(1, 0, new QTableWidgetItem("mult.flv"));
        downloadsTable->setItem(1, 1, new QTableWidgetItem("37.017 MB"));
        downloadsTable->setItem(1, 2, new QTableWidgetItem("94.97%"));
        downloadsTable->setItem(1, 3, new QTableWidgetItem("0 sec"));
        downloadsTable->setItem(1, 4, new QTableWidgetItem("4.67 MB/sec"));
        downloadsTable->setItem(1, 5, new QTableWidgetItem("Oct 13 2..."));
        downloadsTable->setItem(1, 6, new QTableWidgetItem(""));

        // Details panel
        QGroupBox *detailsGroup = new QGroupBox("Download status", this);
        detailsGroup->setMaximumHeight(200);
        
        QGridLayout *detailsLayout = new QGridLayout(detailsGroup);
        
        // URL and status info
        QLabel *urlLabel = new QLabel("URL:", this);
        QLabel *urlValueLabel = new QLabel("http://mirror2.internetdownloadmanager.com/mult.flv", this);
        urlValueLabel->setWordWrap(true);
        
        QLabel *statusLabel = new QLabel("Status:", this);
        QLabel *statusValueLabel = new QLabel("Receiving data...", this);
        
        QLabel *fileSizeLabel = new QLabel("File size:", this);
        QLabel *fileSizeValueLabel = new QLabel("37.017 MB", this);
        
        QLabel *downloadedLabel = new QLabel("Downloaded:", this);
        QLabel *downloadedValueLabel = new QLabel("35.158 MB (94.97 %)", this);
        
        QLabel *transferRateLabel = new QLabel("Transfer rate:", this);
        QLabel *transferRateValueLabel = new QLabel("4.679 MB/sec", this);
        
        QLabel *timeLeftLabel = new QLabel("Time left:", this);
        QLabel *timeLeftValueLabel = new QLabel("0 sec", this);
        
        QLabel *resumeCapabilityLabel = new QLabel("Resume capability:", this);
        QLabel *resumeValueLabel = new QLabel("Yes", this);
        
        // Progress bar
        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setMinimum(0);
        progressBar->setMaximum(100);
        progressBar->setValue(95);
        progressBar->setTextVisible(false);
        
        // Buttons
        QHBoxLayout *buttonsLayout = new QHBoxLayout();
        QPushButton *showDetailsButton = new QPushButton("Show details >>", this);
        QPushButton *pauseButton = new QPushButton("Pause", this);
        QPushButton *cancelButton = new QPushButton("Cancel", this);
        
        buttonsLayout->addWidget(showDetailsButton);
        buttonsLayout->addStretch();
        buttonsLayout->addWidget(pauseButton);
        buttonsLayout->addWidget(cancelButton);
        
        // Layout
        detailsLayout->addWidget(urlLabel, 0, 0);
        detailsLayout->addWidget(urlValueLabel, 0, 1, 1, 3);
        detailsLayout->addWidget(statusLabel, 1, 0);
        detailsLayout->addWidget(statusValueLabel, 1, 1);
        detailsLayout->addWidget(fileSizeLabel, 2, 0);
        detailsLayout->addWidget(fileSizeValueLabel, 2, 1);
        detailsLayout->addWidget(downloadedLabel, 2, 2);
        detailsLayout->addWidget(downloadedValueLabel, 2, 3);
        detailsLayout->addWidget(transferRateLabel, 3, 0);
        detailsLayout->addWidget(transferRateValueLabel, 3, 1);
        detailsLayout->addWidget(timeLeftLabel, 3, 2);
        detailsLayout->addWidget(timeLeftValueLabel, 3, 3);
        detailsLayout->addWidget(resumeCapabilityLabel, 4, 0);
        detailsLayout->addWidget(resumeValueLabel, 4, 1);
        detailsLayout->addWidget(progressBar, 5, 0, 1, 4);
        detailsLayout->addLayout(buttonsLayout, 6, 0, 1, 4);

        // Assemble splitters
        verticalSplitter->addWidget(downloadsTable);
        verticalSplitter->addWidget(detailsGroup);
        verticalSplitter->setSizes({500, 200});

        horizontalSplitter->addWidget(categoriesTree);
        horizontalSplitter->addWidget(verticalSplitter);
        horizontalSplitter->setSizes({200, 800});

        mainLayout->addWidget(horizontalSplitter);

        // Status bar
        QProgressBar *globalProgressBar = new QProgressBar(this);
        globalProgressBar->setMaximumWidth(200);
        globalProgressBar->setVisible(false);
        
        QLabel *mainStatusLabel = new QLabel("Ready", this);
        QLabel *speedLabel = new QLabel("", this);
        
        statusBar()->addWidget(mainStatusLabel);
        statusBar()->addPermanentWidget(speedLabel);
        statusBar()->addPermanentWidget(globalProgressBar);
    }
};

#include "simple_main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SimpleMainWindow window;
    window.show();

    return app.exec();
}