#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QTreeWidget>
#include <QTableWidget>
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
#include <iostream>

class LDMMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LDMMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM - Like Download Manager v1.0.0");
        setMinimumSize(800, 600);
        resize(1000, 700);

        setupMenuBar();
        setupToolBar();
        setupCentralWidget();
        setupStatusBar();

        // Load settings
        loadSettings();

        std::cout << "LDM initialized successfully!" << std::endl;
    }

private slots:
    void onAddUrl()
    {
        bool ok;
        QString url = QInputDialog::getText(this, "Add URL", "Enter download URL:", QLineEdit::Normal, "", &ok);
        if (ok && !url.isEmpty()) {
            QMessageBox::information(this, "URL Added", "URL: " + url + "\nAdded to download queue.");
        }
    }

    void onSettings()
    {
        QMessageBox::information(this, "Settings", "Settings dialog would open here.");
    }

    void onAbout()
    {
        QMessageBox::about(this, "About LDM",
            "LDM - Like Download Manager v1.0.0\n\n"
            "A modern download manager for Linux\n"
            "Developer: Anna-el Gerard RABENANDRASANA (aerab243)\n"
            "Project: https://github.com/aerab243/ldm");
    }

private:
    void setupMenuBar()
    {
        QMenuBar *menuBar = this->menuBar();

        QMenu *fileMenu = menuBar->addMenu("&File");
        fileMenu->addAction("&Add URL", this, &LDMMainWindow::onAddUrl, QKeySequence::New);
        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

        QMenu *toolsMenu = menuBar->addMenu("&Tools");
        toolsMenu->addAction("&Settings", this, &LDMMainWindow::onSettings);

        QMenu *helpMenu = menuBar->addMenu("&Help");
        helpMenu->addAction("&About", this, &LDMMainWindow::onAbout);
    }

    void setupToolBar()
    {
        QToolBar *toolBar = addToolBar("Main");
        toolBar->addAction("Add URL", this, &LDMMainWindow::onAddUrl);
        toolBar->addSeparator();
        toolBar->addAction("Settings", this, &LDMMainWindow::onSettings);
    }

    void setupCentralWidget()
    {
        QWidget *centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

        // Left panel - Categories
        QTreeWidget *categoryTree = new QTreeWidget;
        categoryTree->setHeaderLabel("Categories");
        categoryTree->setMaximumWidth(200);

        QStringList categories = {"All Downloads", "Downloading", "Completed", "Compressed", "Documents", "Music", "Videos", "Programs"};
        for (const QString &category : categories) {
            QTreeWidgetItem *item = new QTreeWidgetItem(categoryTree);
            item->setText(0, category);
        }

        // Right panel - Downloads and details
        QSplitter *rightSplitter = new QSplitter(Qt::Vertical);

        // Downloads table
        QTableWidget *downloadsTable = new QTableWidget(0, 6);
        QStringList headers = {"File Name", "Size", "Downloaded", "Progress", "Speed", "Status"};
        downloadsTable->setHorizontalHeaderLabels(headers);
        downloadsTable->horizontalHeader()->setStretchLastSection(true);

        // Details panel
        QWidget *detailsWidget = new QWidget;
        QVBoxLayout *detailsLayout = new QVBoxLayout(detailsWidget);
        detailsLayout->addWidget(new QLabel("Download Details"));
        detailsLayout->addWidget(new QProgressBar);
        detailsLayout->addWidget(new QLabel("No download selected"));

        rightSplitter->addWidget(downloadsTable);
        rightSplitter->addWidget(detailsWidget);
        rightSplitter->setSizes({400, 200});

        mainLayout->addWidget(categoryTree);
        mainLayout->addWidget(rightSplitter);
        mainLayout->setStretch(1, 1);
    }

    void setupStatusBar()
    {
        statusBar()->showMessage("Ready");
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

protected:
    void closeEvent(QCloseEvent *event) override
    {
        saveSettings();
        QMainWindow::closeEvent(event);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("LDM");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("LDM Project");

    // Handle command line arguments
    if (argc > 1) {
        QString arg = QString(argv[1]);
        if (arg == "--version" || arg == "-v") {
            std::cout << "LDM - Like Download Manager v1.0.0" << std::endl;
            std::cout << "Developer: Anna-el Gerard RABENANDRASANA (aerab243)" << std::endl;
            std::cout << "Project: https://github.com/aerab243/ldm" << std::endl;
            return 0;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "LDM - Like Download Manager v1.0.0" << std::endl;
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --version, -v    Show version information" << std::endl;
            std::cout << "  --help, -h       Show this help message" << std::endl;
            return 0;
        }
    }

    LDMMainWindow window;
    window.show();

    return app.exec();
}

#include "main_simple_notray.moc"
