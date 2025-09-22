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
#include <QRegularExpression>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QBrush>
#include <QColor>

// Simple AddURL Dialog for testing
class SimpleAddUrlDialog : public QDialog
{
    Q_OBJECT

public:
    SimpleAddUrlDialog(QWidget *parent = nullptr);
    
    QString getUrl() const { return m_urlEdit->text(); }
    QString getFilename() const { return m_filenameEdit->text(); }
    QString getReferrer() const { return m_referrerEdit->text(); }
    int getCategoryId() const { return m_categoryCombo->currentIndex(); }

private slots:
    void onUrlChanged(const QString &url);
    void onBrowseClicked();

private:
    QLineEdit *m_urlEdit;
    QLineEdit *m_filenameEdit;
    QLineEdit *m_referrerEdit;
    QComboBox *m_categoryCombo;
    QPushButton *m_browseButton;

public:    
    QString extractFilenameFromUrl(const QString &url);
};

SimpleAddUrlDialog::SimpleAddUrlDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Ajouter un téléchargement - Test AddURL");
    setModal(true);
    resize(500, 300);

    // Create widgets
    m_urlEdit = new QLineEdit(this);
    m_filenameEdit = new QLineEdit(this);
    m_referrerEdit = new QLineEdit(this);
    m_categoryCombo = new QComboBox(this);
    m_browseButton = new QPushButton("Parcourir...", this);
    
    // Setup category combo
    m_categoryCombo->addItem("📁 Tous les téléchargements", 0);
    m_categoryCombo->addItem("📦 Archives compressées", 1);
    m_categoryCombo->addItem("📄 Documents", 2);
    m_categoryCombo->addItem("🎵 Musique", 3);
    m_categoryCombo->addItem("⚙️ Programmes", 4);
    m_categoryCombo->addItem("🎬 Vidéos", 5);

    // Create layout
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("URL:", m_urlEdit);
    
    QHBoxLayout *filenameLayout = new QHBoxLayout();
    filenameLayout->addWidget(m_filenameEdit);
    filenameLayout->addWidget(m_browseButton);
    formLayout->addRow("Nom de fichier:", filenameLayout);
    
    formLayout->addRow("Référent:", m_referrerEdit);
    formLayout->addRow("Catégorie:", m_categoryCombo);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Annuler", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(m_urlEdit, &QLineEdit::textChanged, this, &SimpleAddUrlDialog::onUrlChanged);
    connect(m_browseButton, &QPushButton::clicked, this, &SimpleAddUrlDialog::onBrowseClicked);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // Set placeholder texts
    m_urlEdit->setPlaceholderText("https://example.com/file.zip");
    m_filenameEdit->setPlaceholderText("Le nom sera automatiquement extrait de l'URL");
    m_referrerEdit->setPlaceholderText("https://example.com (optionnel)");
}

void SimpleAddUrlDialog::onUrlChanged(const QString &url)
{
    if (!url.isEmpty() && m_filenameEdit->text().isEmpty()) {
        QString extractedName = extractFilenameFromUrl(url);
        m_filenameEdit->setText(extractedName);
    }
}

void SimpleAddUrlDialog::onBrowseClicked()
{
    // This would open a file dialog in a real implementation
    QMessageBox::information(this, "Info", "Fonctionnalité de navigation non implémentée dans ce test");
}

QString SimpleAddUrlDialog::extractFilenameFromUrl(const QString &url)
{
    if (url.isEmpty()) {
        return QString();
    }
    
    QUrl qurl(url);
    QString path = qurl.path();
    QString filename = QFileInfo(path).fileName();
    
    // If no filename found in path, try to extract from URL parameters
    if (filename.isEmpty()) {
        QRegularExpression filenameRegex(R"(filename=([^&;]+))");
        QRegularExpressionMatch match = filenameRegex.match(url);
        if (match.hasMatch()) {
            filename = match.captured(1);
        }
    }
    
    // If still no filename, generate one from host
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

// Main test window
class AddUrlTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    AddUrlTestWindow(QWidget *parent = nullptr);

private slots:
    void onAddUrlClicked();
    void onTestUrlClicked();
    void onClearHistoryClicked();

private:
    void setupUI();
    void setupTestUrls();
    void addDownloadToTable(const QString &url, const QString &filename, const QString &category);
    void logMessage(const QString &message);

    // UI Components
    QPushButton *m_addUrlButton;
    QPushButton *m_testUrlButton;
    QPushButton *m_clearHistoryButton;
    QLineEdit *m_quickUrlEdit;
    QLabel *m_statusLabel;
    QTextEdit *m_logTextEdit;
    QTableWidget *m_downloadsTable;
    
    // Test data
    QStringList m_testUrls;
    QStringList m_testCategories;
    int m_downloadCounter;
};

AddUrlTestWindow::AddUrlTestWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_downloadCounter(1)
{
    setWindowTitle("LDM AddURL Test - Extraction automatique du nom de fichier");
    setMinimumSize(800, 600);
    
    setupUI();
    setupTestUrls();
    
    logMessage("=== LDM AddURL Test Application Started ===");
    logMessage("Test de la fonctionnalité d'extraction automatique du nom de fichier");
}

void AddUrlTestWindow::setupUI()
{
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Header section
    QGroupBox *headerGroup = new QGroupBox("Test AddURL - Extraction automatique du nom de fichier");
    QVBoxLayout *headerLayout = new QVBoxLayout(headerGroup);
    
    QLabel *descLabel = new QLabel(
        "🎯 Cette application teste la fonctionnalité AddURL avec extraction automatique\n"
        "du nom de fichier depuis l'URL. Le nom de fichier est automatiquement détecté\n"
        "et peut être modifié si nécessaire.\n\n"
        "✨ Fonctionnalités testées:\n"
        "• Extraction du nom de fichier depuis le chemin de l'URL\n"
        "• Génération de nom de fichier depuis le nom d'hôte\n"
        "• Nettoyage des caractères invalides\n"
        "• Support des paramètres filename= dans l'URL"
    );
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("QLabel { background-color: #f0f8ff; padding: 10px; border: 1px solid #ccc; border-radius: 5px; }");
    headerLayout->addWidget(descLabel);
    
    mainLayout->addWidget(headerGroup);
    
    // Control buttons section
    QGroupBox *controlGroup = new QGroupBox("Contrôles de test");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    m_addUrlButton = new QPushButton("🔗 Dialogue AddURL");
    m_addUrlButton->setToolTip("Ouvre le dialogue AddURL pour tester l'extraction du nom de fichier");
    m_addUrlButton->setStyleSheet("QPushButton { font-weight: bold; padding: 8px 16px; }");
    
    m_testUrlButton = new QPushButton("🧪 Test URLs d'exemple");
    m_testUrlButton->setToolTip("Teste avec des URLs d'exemple prédéfinies");
    m_testUrlButton->setStyleSheet("QPushButton { font-weight: bold; padding: 8px 16px; }");
    
    m_clearHistoryButton = new QPushButton("🗑️ Vider l'historique");
    m_clearHistoryButton->setToolTip("Efface l'historique des téléchargements");
    m_clearHistoryButton->setStyleSheet("QPushButton { font-weight: bold; padding: 8px 16px; }");
    
    controlLayout->addWidget(m_addUrlButton);
    controlLayout->addWidget(m_testUrlButton);
    controlLayout->addWidget(m_clearHistoryButton);
    controlLayout->addStretch();
    
    mainLayout->addWidget(controlGroup);
    
    // Quick URL test section
    QGroupBox *quickGroup = new QGroupBox("Test rapide d'extraction");
    QVBoxLayout *quickLayout = new QVBoxLayout(quickGroup);
    
    QLabel *quickLabel = new QLabel("Entrez une URL pour voir l'extraction automatique du nom de fichier:");
    m_quickUrlEdit = new QLineEdit;
    m_quickUrlEdit->setPlaceholderText("https://example.com/path/to/file.zip");
    
    m_statusLabel = new QLabel("Prêt pour les tests");
    m_statusLabel->setStyleSheet("QLabel { font-weight: bold; color: #2E8B57; padding: 5px; }");
    
    quickLayout->addWidget(quickLabel);
    quickLayout->addWidget(m_quickUrlEdit);
    quickLayout->addWidget(m_statusLabel);
    
    mainLayout->addWidget(quickGroup);
    
    // Downloads table
    QGroupBox *tableGroup = new QGroupBox("Historique des tests d'extraction");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    
    m_downloadsTable = new QTableWidget;
    m_downloadsTable->setColumnCount(4);
    QStringList headers = {"#", "URL", "Nom de fichier extrait", "Catégorie"};
    m_downloadsTable->setHorizontalHeaderLabels(headers);
    m_downloadsTable->horizontalHeader()->setStretchLastSection(true);
    m_downloadsTable->setAlternatingRowColors(true);
    m_downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    tableLayout->addWidget(m_downloadsTable);
    mainLayout->addWidget(tableGroup);
    
    // Log section
    QGroupBox *logGroup = new QGroupBox("Journal des tests");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    
    m_logTextEdit = new QTextEdit;
    m_logTextEdit->setMaximumHeight(120);
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setStyleSheet("QTextEdit { font-family: 'Courier New', monospace; }");
    
    logLayout->addWidget(m_logTextEdit);
    mainLayout->addWidget(logGroup);
    
    // Connect signals
    connect(m_addUrlButton, &QPushButton::clicked, this, &AddUrlTestWindow::onAddUrlClicked);
    connect(m_testUrlButton, &QPushButton::clicked, this, &AddUrlTestWindow::onTestUrlClicked);
    connect(m_clearHistoryButton, &QPushButton::clicked, this, &AddUrlTestWindow::onClearHistoryClicked);
    
    // Quick URL test
    connect(m_quickUrlEdit, &QLineEdit::textChanged, [this](const QString &url) {
        if (!url.isEmpty()) {
            SimpleAddUrlDialog dialog;
            QString extractedName = dialog.extractFilenameFromUrl(url);
            m_statusLabel->setText(QString("📁 Nom extrait: <b>%1</b>").arg(extractedName));
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
        "https://www.soundjay.com/misc/sounds/beep-07a.wav",
        "https://httpbin.org/base64/SFRUUEJJTiBpcyBhd2Vzb21l?filename=test.txt",
        "https://example.com/download",
        "https://cdn.example.com/"
    };
    
    m_testCategories = {
        "⚙️ Programmes",
        "📦 Archives",
        "📦 Archives", 
        "📄 Documents",
        "🖼️ Images",
        "🎬 Vidéos",
        "🎵 Musique",
        "📄 Documents",
        "📁 Général",
        "📁 Général"
    };
}

void AddUrlTestWindow::onAddUrlClicked()
{
    SimpleAddUrlDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString url = dialog.getUrl();
        QString filename = dialog.getFilename();
        QString referrer = dialog.getReferrer();
        int categoryId = dialog.getCategoryId();
        
        if (!url.isEmpty()) {
            QString categoryName = "📁 Catégorie " + QString::number(categoryId);
            if (categoryId < 6) {
                QStringList cats = {"📁 Tous", "📦 Archives", "📄 Documents", "🎵 Musique", "⚙️ Programmes", "🎬 Vidéos"};
                categoryName = cats[categoryId];
            }
            
            addDownloadToTable(url, filename, categoryName);
            
            logMessage(QString("✅ URL ajoutée via dialogue: %1").arg(url));
            logMessage(QString("📁 Nom de fichier: %1").arg(filename));
            if (!referrer.isEmpty()) {
                logMessage(QString("🔗 Référent: %1").arg(referrer));
            }
            logMessage(QString("📂 Catégorie: %1").arg(categoryName));
        }
    }
}

void AddUrlTestWindow::onTestUrlClicked()
{
    static int testIndex = 0;
    
    if (testIndex >= m_testUrls.size()) {
        testIndex = 0;
        logMessage("🔄 Redémarrage du cycle de test");
    }
    
    QString testUrl = m_testUrls[testIndex];
    QString testCategory = m_testCategories[testIndex];
    
    SimpleAddUrlDialog dialog;
    QString extractedName = dialog.extractFilenameFromUrl(testUrl);
    
    addDownloadToTable(testUrl, extractedName, testCategory);
    
    logMessage(QString("🧪 Test URL %1/%2: %3").arg(testIndex + 1).arg(m_testUrls.size()).arg(testUrl));
    logMessage(QString("📁 Nom extrait: %1").arg(extractedName));
    
    m_statusLabel->setText(QString("🧪 Test %1/%2 - Fichier: <b>%3</b>")
                          .arg(testIndex + 1).arg(m_testUrls.size()).arg(extractedName));
                          
    testIndex++;
}

void AddUrlTestWindow::onClearHistoryClicked()
{
    m_downloadsTable->setRowCount(0);
    m_downloadCounter = 1;
    m_logTextEdit->clear();
    logMessage("🗑️ Historique effacé - Prêt pour de nouveaux tests");
    m_statusLabel->setText("Prêt pour les tests");
}

void AddUrlTestWindow::addDownloadToTable(const QString &url, const QString &filename, const QString &category)
{
    int row = m_downloadsTable->rowCount();
    m_downloadsTable->insertRow(row);
    
    m_downloadsTable->setItem(row, 0, new QTableWidgetItem(QString::number(m_downloadCounter++)));
    m_downloadsTable->setItem(row, 1, new QTableWidgetItem(url));
    m_downloadsTable->setItem(row, 2, new QTableWidgetItem(filename));
    m_downloadsTable->setItem(row, 3, new QTableWidgetItem(category));
    
    // Style the filename cell to highlight it
    QTableWidgetItem *filenameItem = m_downloadsTable->item(row, 2);
    filenameItem->setBackground(QBrush(QColor("#e8f5e8")));
    filenameItem->setToolTip("Nom de fichier extrait automatiquement depuis l'URL");
    
    // Auto-resize columns
    m_downloadsTable->resizeColumnsToContents();
    
    // Scroll to the new row
    m_downloadsTable->scrollToItem(filenameItem);
}

void AddUrlTestWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, message);
    m_logTextEdit->append(logEntry);
    
    // Auto-scroll to bottom
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logTextEdit->setTextCursor(cursor);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("LDM AddURL Test");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("LDM Project");
    
    AddUrlTestWindow window;
    window.show();
    
    return app.exec();
}

#include "test_addurl_simple.moc"