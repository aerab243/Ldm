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

class TestMainWindow : public QMainWindow
{
public:
    TestMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM Test - Like Download Manager");
        setMinimumSize(1000, 700);
        resize(1200, 800);

        setupUI();
    }

private:
    void setupUI()
    {
        // Create toolbar without icons first
        QToolBar *toolbar = addToolBar("Main");
        toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        
        // Add simple text actions
        toolbar->addAction("Add URL");
        toolbar->addAction("Resume");
        toolbar->addAction("Stop");
        toolbar->addSeparator();
        toolbar->addAction("Options");

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
        
        QTreeWidgetItem *music = new QTreeWidgetItem(categoriesTree);
        music->setText(0, "Music");
        
        QTreeWidgetItem *video = new QTreeWidgetItem(categoriesTree);
        video->setText(0, "Video");
        
        categoriesTree->setCurrentItem(allDownloads);

        // Downloads table
        QTableWidget *downloadsTable = new QTableWidget(this);
        
        QStringList headers;
        headers << "File Name" << "Size" << "Status" << "Time left" << "Transfer rate" << "Last Try" << "Description";
        downloadsTable->setColumnCount(headers.size());
        downloadsTable->setHorizontalHeaderLabels(headers);
        
        downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        downloadsTable->setAlternatingRowColors(true);
        downloadsTable->verticalHeader()->setVisible(false);
        
        // Add sample data
        downloadsTable->insertRow(0);
        downloadsTable->setItem(0, 0, new QTableWidgetItem("sample_file.mp3"));
        downloadsTable->setItem(0, 1, new QTableWidgetItem("5.2 MB"));
        downloadsTable->setItem(0, 2, new QTableWidgetItem("Complete"));
        downloadsTable->setItem(0, 3, new QTableWidgetItem(""));
        downloadsTable->setItem(0, 4, new QTableWidgetItem(""));
        downloadsTable->setItem(0, 5, new QTableWidgetItem("Nov 08"));
        downloadsTable->setItem(0, 6, new QTableWidgetItem(""));

        // Details panel
        QGroupBox *detailsGroup = new QGroupBox("Download status", this);
        detailsGroup->setMaximumHeight(200);
        
        QVBoxLayout *detailsLayout = new QVBoxLayout(detailsGroup);
        
        QLabel *urlLabel = new QLabel("URL: http://example.com/sample_file.mp3", this);
        QLabel *detailsStatusLabel = new QLabel("Status: Complete", this);
        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setValue(100);
        
        QHBoxLayout *buttonsLayout = new QHBoxLayout();
        QPushButton *detailsButton = new QPushButton("Show details >>", this);
        QPushButton *pauseButton = new QPushButton("Pause", this);
        QPushButton *cancelButton = new QPushButton("Cancel", this);
        
        buttonsLayout->addWidget(detailsButton);
        buttonsLayout->addStretch();
        buttonsLayout->addWidget(pauseButton);
        buttonsLayout->addWidget(cancelButton);
        
        detailsLayout->addWidget(urlLabel);
        detailsLayout->addWidget(detailsStatusLabel);
        detailsLayout->addWidget(progressBar);
        detailsLayout->addLayout(buttonsLayout);

        // Assemble splitters
        verticalSplitter->addWidget(downloadsTable);
        verticalSplitter->addWidget(detailsGroup);
        verticalSplitter->setSizes({500, 200});

        horizontalSplitter->addWidget(categoriesTree);
        horizontalSplitter->addWidget(verticalSplitter);
        horizontalSplitter->setSizes({200, 800});

        mainLayout->addWidget(horizontalSplitter);

        // Status bar
        QLabel *statusLabel = new QLabel("Ready", this);
        statusBar()->addWidget(statusLabel);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMainWindow window;
    window.show();

    return app.exec();
}