#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileInfo>
#include <QUrl>
#include <QTimer>
#include <QProgressBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QDebug>

// Include core components
#include "core/DownloadEngine.h"
#include "core/DownloadItem.h"
#include "core/Database.h"
#include "ui/AddUrlDialog.h"

class AddUrlTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    AddUrlTestWindow(QWidget *parent = nullptr);
    ~AddUrlTestWindow();

private slots:
    void onAddUrlClicked();
    void onTestUrlClicked();
    void onClearHistoryClicked();
    void onDownloadStarted(int downloadId);
    void onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadCompleted(int downloadId);
    void onDownloadFailed(int downloadId, const QString &error);

private:
    void setupUI();
    void setupTestUrls();
    void addDownloadToTable(DownloadItem *item);
    void updateDownloadInTable(DownloadItem *item);
    QString extractFilenameFromUrl(const QString &url);
    void logMessage(const QString &message);

    // UI Components
    QWidget *m_centralWidget;
    QPushButton *m_addUrlButton;
    QPushButton *m_testUrlButton;
    QPushButton *m_clearHistoryButton;
    QLineEdit *m_quickUrlEdit;
    QLabel *m_statusLabel;
    QTextEdit *m_logTextEdit;
    QTableWidget *m_downloadsTable;
    QProgressBar *m_globalProgressBar;
    
    // Test URLs
    QStringList m_testUrls;
    
    // Core components
    DownloadEngine *m_downloadEngine;
    Database *m_database;
    AddUrlDialog *m_addUrlDialog;
    
    // Download tracking
    QHash<int, int> m_downloadToRow;
    int m_nextDownloadId;
};

AddUrlTestWindow::AddUrlTestWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_downloadEngine(nullptr)
    , m_database(nullptr)
    , m_addUrlDialog(nullptr)
    , m_nextDownloadId(1)
{
    setWindowTitle("LDM AddURL Test - Test de la fonctionnalité d'ajout d'URL");
    setMinimumSize(800, 600);
    
    // Initialize core components
    m_database = new Database(this);
    m_downloadEngine = new DownloadEngine(this);
    
    // Connect download engine signals
    connect(m_downloadEngine, &DownloadEngine::downloadStarted,
            this, &AddUrlTestWindow::onDownloadStarted);
    connect(m_downloadEngine, &DownloadEngine::downloadProgress,
            this, &AddUrlTestWindow::onDownloadProgress);
    connect(m_downloadEngine, &DownloadEngine::downloadCompleted,
            this, &AddUrlTestWindow::onDownloadCompleted);
    connect(m_downloadEngine, &DownloadEngine::downloadFailed,
            this, &AddUrlTestWindow::onDownloadFailed);
    
    setupUI();
    setupTestUrls();
    
    logMessage("=== LDM AddURL Test Application Started ===");
    logMessage("Test de la fonctionnalité d'extraction automatique du nom de fichier");
}

AddUrlTestWindow::~AddUrlTestWindow()
{
}

void AddUrlTestWindow::setupUI()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    
    // Header section
    QGroupBox *headerGroup = new QGroupBox("Test AddURL - Extraction automatique du nom de fichier");
    QVBoxLayout *headerLayout = new QVBoxLayout(headerGroup);
    
    QLabel *descLabel = new QLabel(
        "Cette application teste la fonctionnalité AddURL avec extraction automatique\n"
        "du nom de fichier depuis l'URL. Le nom de fichier est automatiquement détecté\n"
        "et peut être modifié si nécessaire."
    );
    descLabel->setWordWrap(true);
    headerLayout->addWidget(descLabel);
    
    mainLayout->addWidget(headerGroup);
    
    // Control buttons section
    QGroupBox *controlGroup = new QGroupBox("Contrôles de test");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    m_addUrlButton = new QPushButton("🔗 Ajouter URL (AddURL Dialog)");
    m_addUrlButton->setToolTip("Ouvre le dialogue AddURL pour tester l'extraction du nom de fichier");
    
    m_testUrlButton = new QPushButton("🧪 Test URLs d'exemple");
    m_testUrlButton->setToolTip("Teste avec des URLs d'exemple prédéfinies");
    
    m_clearHistoryButton = new QPushButton("🗑️ Vider l'historique");
    m_clearHistoryButton->setToolTip("Efface l'historique des téléchargements");
    
    controlLayout->addWidget(m_addUrlButton);
    controlLayout->addWidget(m_testUrlButton);
    controlLayout->addWidget(m_clearHistoryButton);
    controlLayout->addStretch();
    
    mainLayout->addWidget(controlGroup);
    
    // Quick URL test section
    QGroupBox *quickGroup = new QGroupBox("Test rapide d'URL");
    QVBoxLayout *quickLayout = new QVBoxLayout(quickGroup);
    
    QLabel *quickLabel = new QLabel("Entrez une URL pour tester l'extraction du nom de fichier:");
    m_quickUrlEdit = new QLineEdit;
    m_quickUrlEdit->setPlaceholderText("https://example.com/path/to/file.zip");
    
    quickLayout->addWidget(quickLabel);
    quickLayout->addWidget(m_quickUrlEdit);
    
    mainLayout->addWidget(quickGroup);
    
    // Downloads table
    QGroupBox *tableGroup = new QGroupBox("Téléchargements avec noms de fichiers extraits");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    
    m_downloadsTable = new QTableWidget;
    m_downloadsTable->setColumnCount(5);
    QStringList headers = {"ID", "URL", "Nom de fichier", "Statut", "Progression"};
    m_downloadsTable->setHorizontalHeaderLabels(headers);
    m_downloadsTable->horizontalHeader()->setStretchLastSection(true);
    m_downloadsTable->setAlternatingRowColors(true);
    m_downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    tableLayout->addWidget(m_downloadsTable);
    mainLayout->addWidget(tableGroup);
    
    // Global progress
    m_globalProgressBar = new QProgressBar;
    m_globalProgressBar->setVisible(false);
    mainLayout->addWidget(m_globalProgressBar);
    
    // Status and log section
    QGroupBox *logGroup = new QGroupBox("Journal des tests");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    
    m_statusLabel = new QLabel("Prêt pour les tests");
    m_statusLabel->setStyleSheet("QLabel { font-weight: bold; color: #2E8B57; }");
    
    m_logTextEdit = new QTextEdit;
    m_logTextEdit->setMaximumHeight(150);
    m_logTextEdit->setReadOnly(true);
    
    logLayout->addWidget(m_statusLabel);
    logLayout->addWidget(m_logTextEdit);
    
    mainLayout->addWidget(logGroup);
    
    // Connect signals
    connect(m_addUrlButton, &QPushButton::clicked, this, &AddUrlTestWindow::onAddUrlClicked);
    connect(m_testUrlButton, &QPushButton::clicked, this, &AddUrlTestWindow::onTestUrlClicked);
    connect(m_clearHistoryButton, &QPushButton::clicked, this, &AddUrlTestWindow::onClearHistoryClicked);
    
    // Quick URL test
    connect(m_quickUrlEdit, &QLineEdit::textChanged, [this](const QString &url) {
        if (!url.isEmpty()) {
            QString extractedName = extractFilenameFromUrl(url);
            m_statusLabel->setText(QString("Nom extrait: %1").arg(extractedName));
        } else {
            m_statusLabel->setText("Prêt pour les tests");
        }
    });
}

void AddUrlTestWindow::setupTestUrls()
{
    m_testUrls = {
        "https://releases.ubuntu.com/22.04/ubuntu-22.04.3-desktop-amd64.iso",
        "https://download.mozilla.org/pub/firefox/releases/latest/linux-x86_64/en-US/firefox-latest.tar.bz2",
        "https://github.com/torvalds/linux/archive/refs/heads/master.zip",
        "https://www.w3.org/WAI/ER/tests/xhtml/testfiles/resources/pdf/dummy.pdf",
        "https://file-examples.com/storage/fe68c16bb66071ae2c83f87/2017/10/file_example_JPG_100kB.jpg",
        "https://sample-videos.com/zip/10/mp4/SampleVideo_1280x720_1mb.mp4",
        "https://www.soundjay.com/misc/sounds/beep-07a.wav"
    };
}

void AddUrlTestWindow::onAddUrlClicked()
{
    if (!m_addUrlDialog) {
        m_addUrlDialog = new AddUrlDialog(m_database, this);
    }
    
    if (m_addUrlDialog->exec() == QDialog::Accepted) {
        QString url = m_addUrlDialog->getUrl();
        QString filename = m_addUrlDialog->getFileName();
        int categoryId = m_addUrlDialog->getCategoryId();
        
        if (!url.isEmpty()) {
            // Create download item
            DownloadItem *item = new DownloadItem(m_nextDownloadId++, url, filename, this);
            item->setStatus("En attente");
            item->setCategoryId(categoryId);
            item->setCreatedAt(QDateTime::currentDateTime());
            
            addDownloadToTable(item);
            
            logMessage(QString("✅ URL ajoutée: %1").arg(url));
            logMessage(QString("📁 Nom de fichier: %1").arg(filename));
            logMessage(QString("📂 Catégorie ID: %1").arg(categoryId));
            
            // Start download (simulation)
            m_downloadEngine->startDownload(item);
        }
    }
}

void AddUrlTestWindow::onTestUrlClicked()
{
    static int testIndex = 0;
    
    if (testIndex >= m_testUrls.size()) {
        testIndex = 0;
    }
    
    QString testUrl = m_testUrls[testIndex++];
    QString extractedName = extractFilenameFromUrl(testUrl);
    
    // Create download item
    DownloadItem *item = new DownloadItem(m_nextDownloadId++, testUrl, extractedName, this);
    item->setStatus("Test");
    item->setCreatedAt(QDateTime::currentDateTime());
    
    addDownloadToTable(item);
    
    logMessage(QString("🧪 URL de test: %1").arg(testUrl));
    logMessage(QString("📁 Nom extrait: %1").arg(extractedName));
    
    m_statusLabel->setText(QString("Test URL %1/%2 - Nom: %3")
                          .arg(testIndex).arg(m_testUrls.size()).arg(extractedName));
}

void AddUrlTestWindow::onClearHistoryClicked()
{
    m_downloadsTable->setRowCount(0);
    m_downloadToRow.clear();
    m_nextDownloadId = 1;
    logMessage("🗑️ Historique effacé");
    m_statusLabel->setText("Prêt pour les tests");
}

void AddUrlTestWindow::addDownloadToTable(DownloadItem *item)
{
    int row = m_downloadsTable->rowCount();
    m_downloadsTable->insertRow(row);
    
    m_downloadsTable->setItem(row, 0, new QTableWidgetItem(QString::number(item->getId())));
    m_downloadsTable->setItem(row, 1, new QTableWidgetItem(item->getUrl()));
    m_downloadsTable->setItem(row, 2, new QTableWidgetItem(item->getFilename()));
    m_downloadsTable->setItem(row, 3, new QTableWidgetItem(item->getStatus()));
    m_downloadsTable->setItem(row, 4, new QTableWidgetItem("0%"));
    
    m_downloadToRow[item->getId()] = row;
    
    // Auto-resize columns
    m_downloadsTable->resizeColumnsToContents();
}

void AddUrlTestWindow::updateDownloadInTable(DownloadItem *item)
{
    if (m_downloadToRow.contains(item->getId())) {
        int row = m_downloadToRow[item->getId()];
        m_downloadsTable->item(row, 3)->setText(item->getStatus());
        m_downloadsTable->item(row, 4)->setText(QString("%1%").arg(item->getProgress(), 0, 'f', 1));
    }
}

QString AddUrlTestWindow::extractFilenameFromUrl(const QString &url)
{
    if (url.isEmpty()) {
        return "download";
    }
    
    QUrl qurl(url);
    QString path = qurl.path();
    QString filename = QFileInfo(path).fileName();
    
    // If no filename found in path, generate one from host
    if (filename.isEmpty()) {
        QString host = qurl.host();
        if (!host.isEmpty()) {
            filename = host + "_download";
        } else {
            filename = "download";
        }
    }
    
    // Clean up filename (remove invalid characters)
    filename = filename.replace(QRegularExpression(R"([<>:"/\\|?*])"), "_");
    
    return filename;
}

void AddUrlTestWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);
    m_logTextEdit->append(logEntry);
    
    // Auto-scroll to bottom
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logTextEdit->setTextCursor(cursor);
}

void AddUrlTestWindow::onDownloadStarted(int downloadId)
{
    DownloadItem *item = m_downloadEngine->getDownload(downloadId);
    if (item) {
        item->setStatus("Téléchargement...");
        updateDownloadInTable(item);
        logMessage(QString("🚀 Téléchargement démarré: ID %1").arg(downloadId));
    }
}

void AddUrlTestWindow::onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal)
{
    DownloadItem *item = m_downloadEngine->getDownload(downloadId);
    if (item && bytesTotal > 0) {
        double progress = (double)bytesReceived / bytesTotal * 100.0;
        item->setDownloadedSize(bytesReceived);
        item->setTotalSize(bytesTotal);
        updateDownloadInTable(item);
    }
}

void AddUrlTestWindow::onDownloadCompleted(int downloadId)
{
    DownloadItem *item = m_downloadEngine->getDownload(downloadId);
    if (item) {
        item->setStatus("Terminé");
        item->setCompletedAt(QDateTime::currentDateTime());
        updateDownloadInTable(item);
        logMessage(QString("✅ Téléchargement terminé: %1").arg(item->getFilename()));
    }
}

void AddUrlTestWindow::onDownloadFailed(int downloadId, const QString &error)
{
    DownloadItem *item = m_downloadEngine->getDownload(downloadId);
    if (item) {
        item->setStatus("Erreur");
        item->setErrorMessage(error);
        updateDownloadInTable(item);
        logMessage(QString("❌ Téléchargement échoué: %1 - %2").arg(item->getFilename()).arg(error));
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    AddUrlTestWindow window;
    window.show();
    
    return app.exec();
}

#include "test_addurl.moc"