#!/bin/bash

# Build script for testing the IDM-style interface standalone
# This creates a simple executable to test the interface without complex dependencies

set -e

echo "Building IDM-style interface test..."

# Ensure we're in the right directory
cd "$(dirname "$0")"

# Create a simplified test version
cat > idm_test.cpp << 'EOF'
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

class LDMMainWindow : public QMainWindow
{
public:
    LDMMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM - Like Download Manager 1.0.0");
        setMinimumSize(1000, 700);
        resize(1200, 800);
        setupUI();
    }

private:
    void setupUI()
    {
        // Menu bar
        QMenuBar *menuBar = this->menuBar();
        QMenu *tasksMenu = menuBar->addMenu("&Tasks");
        tasksMenu->addAction("&Add URL...");
        QMenu *downloadsMenu = menuBar->addMenu("&Downloads");
        downloadsMenu->addAction("&Resume");
        downloadsMenu->addAction("&Pause");
        downloadsMenu->addAction("&Stop");
        QMenu *viewMenu = menuBar->addMenu("&View");
        viewMenu->addAction("&Toolbar");
        viewMenu->addAction("&Status Bar");
        QMenu *helpMenu = menuBar->addMenu("&Help");
        helpMenu->addAction("&About LDM");

        // Toolbar
        QToolBar *toolbar = addToolBar("Main");
        toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        toolbar->addAction("Add URL");
        toolbar->addAction("Resume");
        toolbar->addAction("Stop");
        toolbar->addAction("Stop All");
        toolbar->addAction("Delete");
        toolbar->addAction("Delete C...");
        toolbar->addSeparator();
        toolbar->addAction("Options");
        toolbar->addAction("Scheduler");
        toolbar->addSeparator();
        toolbar->addAction("Start Qu...");
        toolbar->addAction("Stop Qu...");
        toolbar->addAction("Grabber");
        toolbar->addSeparator();
        toolbar->addAction("Tell a Frie...");

        // Central widget with splitters
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(2, 2, 2, 2);
        mainLayout->setSpacing(2);

        QSplitter *horizontalSplitter = new QSplitter(Qt::Horizontal, this);
        QSplitter *verticalSplitter = new QSplitter(Qt::Vertical, this);

        // Categories tree
        QTreeWidget *categoriesTree = new QTreeWidget(this);
        categoriesTree->setHeaderLabel("Categories");
        categoriesTree->setMaximumWidth(250);
        categoriesTree->setMinimumWidth(180);

        QTreeWidgetItem *allDownloads = new QTreeWidgetItem(categoriesTree);
        allDownloads->setText(0, "📁 All Downloads");
        QTreeWidgetItem *compressed = new QTreeWidgetItem(categoriesTree);
        compressed->setText(0, "📦 Compressed");
        QTreeWidgetItem *documents = new QTreeWidgetItem(categoriesTree);
        documents->setText(0, "📄 Documents");
        QTreeWidgetItem *music = new QTreeWidgetItem(categoriesTree);
        music->setText(0, "🎵 Music");
        QTreeWidgetItem *programs = new QTreeWidgetItem(categoriesTree);
        programs->setText(0, "⚙ Programs");
        QTreeWidgetItem *video = new QTreeWidgetItem(categoriesTree);
        video->setText(0, "🎬 Video");
        QTreeWidgetItem *unfinished = new QTreeWidgetItem(categoriesTree);
        unfinished->setText(0, "⏳ Unfinished");
        QTreeWidgetItem *finished = new QTreeWidgetItem(categoriesTree);
        finished->setText(0, "✓ Finished");
        QTreeWidgetItem *queues = new QTreeWidgetItem(categoriesTree);
        queues->setText(0, "📋 Queues");
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

        QHeaderView *header = downloadsTable->horizontalHeader();
        header->setStretchLastSection(true);
        downloadsTable->setColumnWidth(0, 200);
        downloadsTable->setColumnWidth(1, 80);
        downloadsTable->setColumnWidth(2, 100);
        downloadsTable->setColumnWidth(3, 80);
        downloadsTable->setColumnWidth(4, 100);
        downloadsTable->setColumnWidth(5, 120);

        // Sample data
        downloadsTable->insertRow(0);
        downloadsTable->setItem(0, 0, new QTableWidgetItem("♪ undercover.mp3"));
        downloadsTable->setItem(0, 1, new QTableWidgetItem("7.16 MB"));
        downloadsTable->setItem(0, 2, new QTableWidgetItem("Complete"));
        downloadsTable->setItem(0, 3, new QTableWidgetItem(""));
        downloadsTable->setItem(0, 4, new QTableWidgetItem(""));
        downloadsTable->setItem(0, 5, new QTableWidgetItem("Nov 08 ..."));
        downloadsTable->setItem(0, 6, new QTableWidgetItem(""));

        downloadsTable->insertRow(1);
        downloadsTable->setItem(1, 0, new QTableWidgetItem("🎬 mult.flv"));
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

        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setMinimum(0);
        progressBar->setMaximum(100);
        progressBar->setValue(95);
        progressBar->setTextVisible(false);

        QHBoxLayout *buttonsLayout = new QHBoxLayout();
        QPushButton *showDetailsButton = new QPushButton("Show details >>", this);
        QPushButton *pauseButton = new QPushButton("Pause", this);
        QPushButton *cancelButton = new QPushButton("Cancel", this);
        buttonsLayout->addWidget(showDetailsButton);
        buttonsLayout->addStretch();
        buttonsLayout->addWidget(pauseButton);
        buttonsLayout->addWidget(cancelButton);

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
        globalProgressBar->setValue(95);
        globalProgressBar->setFormat("95%");
        QLabel *mainStatusLabel = new QLabel("Ready - 1 download active", this);
        QLabel *speedLabel = new QLabel("4.67 MB/sec", this);
        statusBar()->addWidget(mainStatusLabel);
        statusBar()->addPermanentWidget(speedLabel);
        statusBar()->addPermanentWidget(globalProgressBar);

        // IDM-style styling
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("LDM");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");

    LDMMainWindow window;
    window.show();

    return app.exec();
}
EOF

echo "Compiling IDM test interface..."

# Find Qt6 includes and libs
QT6_INC="/usr/include/qt6"
QT6_LIBS="-lQt6Core -lQt6Widgets -lQt6Gui"

# Compile
g++ -fPIC \
    -I"$QT6_INC" \
    -I"$QT6_INC/QtCore" \
    -I"$QT6_INC/QtWidgets" \
    -I"$QT6_INC/QtGui" \
    idm_test.cpp \
    $QT6_LIBS \
    -o ldm_idm_test

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo "✓ Executable created: ldm_idm_test"
    echo ""
    echo "To run the test interface:"
    echo "  DISPLAY=:0 ./ldm_idm_test"
    echo ""
    echo "This demonstrates the IDM-style interface without complex dependencies."
else
    echo "✗ Build failed!"
    exit 1
fi

# Cleanup
rm -f idm_test.cpp

echo "IDM interface test build complete!"
