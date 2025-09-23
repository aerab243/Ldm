#include "AddUrlDialog.h"
#include "../core/Database.h"
#include <QFileDialog>
#include <QUrl>
#include <QFileInfo>
#include <QRegularExpression>

AddUrlDialog::AddUrlDialog(Database *database, QWidget *parent)
    : QDialog(parent), m_database(database)
{
    setupUI();
    loadCategories();
}

AddUrlDialog::AddUrlDialog(const QString &url, QWidget *parent)
    : QDialog(parent), m_database(nullptr)
{
    setupUI();
    if (!url.isEmpty()) {
        m_urlEdit->setText(url);
    }
}

AddUrlDialog::~AddUrlDialog()
{
}

QString AddUrlDialog::getUrl() const
{
    return m_urlEdit->text();
}

QString AddUrlDialog::getFileName() const
{
    return m_fileNameEdit->text();
}

// Stub implementations for missing methods
void AddUrlDialog::setUrl(const QString &url) { m_urlEdit->setText(url); }
void AddUrlDialog::setFilename(const QString &filename) { m_fileNameEdit->setText(filename); }
void AddUrlDialog::setSavePath(const QString &path) { /* stub */ }
QString AddUrlDialog::getSavePath() const { return QString(); }
QString AddUrlDialog::getFullFilePath() const { return QString(); }
void AddUrlDialog::setCategory(const QString &category) { /* stub */ }
QString AddUrlDialog::getCategory() const { return QString(); }
void AddUrlDialog::setStartDownload(bool start) { /* stub */ }
bool AddUrlDialog::shouldStartDownload() const { return true; }
void AddUrlDialog::setScheduleDownload(bool schedule) { /* stub */ }
bool AddUrlDialog::shouldScheduleDownload() const { return false; }
void AddUrlDialog::setSegmentCount(int count) { /* stub */ }
int AddUrlDialog::getSegmentCount() const { return 8; }
void AddUrlDialog::setSpeedLimit(int limit) { /* stub */ }
int AddUrlDialog::getSpeedLimit() const { return 0; }
void AddUrlDialog::setProxySettings(const QString &, int, const QString &, const QString &) { /* stub */ }
void AddUrlDialog::setUserAgent(const QString &) { /* stub */ }
QString AddUrlDialog::getUserAgent() const { return QString(); }
void AddUrlDialog::setReferer(const QString &) { /* stub */ }
QString AddUrlDialog::getReferer() const { return QString(); }
void AddUrlDialog::setCookies(const QString &) { /* stub */ }
QString AddUrlDialog::getCookies() const { return QString(); }
void AddUrlDialog::setHeaders(const QStringList &) { /* stub */ }
QStringList AddUrlDialog::getHeaders() const { return QStringList(); }
void AddUrlDialog::resetToDefaults() { /* stub */ }
void AddUrlDialog::loadFromClipboard() { /* stub */ }
void AddUrlDialog::saveSettings() { /* stub */ }
void AddUrlDialog::loadSettings() { /* stub */ }
void AddUrlDialog::showEvent(QShowEvent *) { /* stub */ }
void AddUrlDialog::closeEvent(QCloseEvent *) { /* stub */ }
void AddUrlDialog::keyPressEvent(QKeyEvent *) { /* stub */ }
void AddUrlDialog::dragEnterEvent(QDragEnterEvent *) { /* stub */ }
void AddUrlDialog::dropEvent(QDropEvent *) { /* stub */ }
AddUrlDialog::DownloadOptions AddUrlDialog::getDownloadOptions() const { return DownloadOptions(); }

// Slot stubs
void AddUrlDialog::onUrlChanged() { /* stub */ }
void AddUrlDialog::onUrlTextChanged(const QString &) { /* stub */ }
void AddUrlDialog::onPasteUrlClicked() { /* stub */ }
void AddUrlDialog::onClearUrlClicked() { /* stub */ }
void AddUrlDialog::onValidateUrlClicked() { /* stub */ }
void AddUrlDialog::onFilenameChanged() { /* stub */ }
void AddUrlDialog::onBrowseSavePathClicked() { /* stub */ }
void AddUrlDialog::onOpenFolderClicked() { /* stub */ }
void AddUrlDialog::onCategoryChanged() { /* stub */ }
void AddUrlDialog::onDownloadLaterClicked() { /* stub */ }
void AddUrlDialog::onScheduleClicked() { /* stub */ }
void AddUrlDialog::onResetClicked() { /* stub */ }
void AddUrlDialog::onAdvancedToggled() { /* stub */ }
void AddUrlDialog::onTabChanged(int) { /* stub */ }
void AddUrlDialog::onUrlValidationFinished() { /* stub */ }
void AddUrlDialog::onFileInfoReceived() { /* stub */ }
void AddUrlDialog::onNetworkError(QNetworkReply::NetworkError) { /* stub */ }
void AddUrlDialog::onUrlCompletionActivated(const QString &) { /* stub */ }
void AddUrlDialog::onPathCompletionActivated(const QString &) { /* stub */ }
void AddUrlDialog::onSegmentCountChanged(int) { /* stub */ }
void AddUrlDialog::onSpeedLimitChanged(int) { /* stub */ }
void AddUrlDialog::onProxySettingsChanged() { /* stub */ }
void AddUrlDialog::onAdvancedSettingsChanged() { /* stub */ }

// UrlValidator implementation
UrlValidator::UrlValidator(QObject *parent) : QValidator(parent) {}
QValidator::State UrlValidator::validate(QString &input, int &pos) const {
    Q_UNUSED(pos);
    if (input.isEmpty()) return QValidator::Intermediate;
    QUrl url(input);
    return url.isValid() ? QValidator::Acceptable : QValidator::Invalid;
}
bool UrlValidator::isValidUrl(const QString &url) const {
    QUrl qurl(url);
    return qurl.isValid();
}
QUrl UrlValidator::normalizeUrl(const QString &url) const {
    return QUrl(url);
}

// FileTypeDetector implementation
FileTypeDetector::FileTypeDetector(QObject *parent) : QObject(parent) {}
QString FileTypeDetector::detectCategory(const QString &filename) const {
    Q_UNUSED(filename);
    return "General";
}
QString FileTypeDetector::detectCategory(const QString &filename, const QString &mimeType) const {
    Q_UNUSED(filename); Q_UNUSED(mimeType);
    return "General";
}
QIcon FileTypeDetector::getFileTypeIcon(const QString &filename) const {
    Q_UNUSED(filename);
    return QIcon();
}
QString FileTypeDetector::getFileDescription(const QString &filename, const QString &mimeType) const {
    Q_UNUSED(filename); Q_UNUSED(mimeType);
    return "File";
}

int AddUrlDialog::getCategoryId() const
{
    return m_categoryCombo->currentData().toInt();
}

void AddUrlDialog::setupUI()
{
    setWindowTitle("Add Download");
    setModal(true);

    m_urlEdit = new QLineEdit(this);
    m_fileNameEdit = new QLineEdit(this);
    m_categoryCombo = new QComboBox(this);
    m_browseButton = new QPushButton("Browse...", this);
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("URL:", m_urlEdit);
    formLayout->addRow("File Name:", m_fileNameEdit);
    formLayout->addRow("Category:", m_categoryCombo);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLayout->addWidget(m_fileNameEdit);
    fileLayout->addWidget(m_browseButton);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(m_browseButton, &QPushButton::clicked, this, &AddUrlDialog::onBrowseClicked);
    connect(m_okButton, &QPushButton::clicked, this, &AddUrlDialog::onOkClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &AddUrlDialog::onCancelClicked);

    // Auto-extract filename when URL changes
    connect(m_urlEdit, &QLineEdit::textChanged, this, [this](const QString &url) {
        if (!url.isEmpty() && m_fileNameEdit->text().isEmpty()) {
            QString extractedName = extractFilenameFromUrl(url);
            m_fileNameEdit->setText(extractedName);
        }
    });
}

void AddUrlDialog::loadCategories()
{
    m_categoryCombo->clear();

    if (m_database) {
        QVariantList categories = m_database->getCategories();
        for (const QVariant &var : categories) {
            QVariantMap category = var.toMap();
            m_categoryCombo->addItem(category["name"].toString(), category["id"].toInt());
        }
    } else {
        // Add default categories when no database is available
        m_categoryCombo->addItem("General", 1);
        m_categoryCombo->addItem("Software", 2);
        m_categoryCombo->addItem("Music", 3);
        m_categoryCombo->addItem("Video", 4);
        m_categoryCombo->addItem("Documents", 5);
    }
}

void AddUrlDialog::onBrowseClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "All Files (*)");
    if (!fileName.isEmpty()) {
        m_fileNameEdit->setText(fileName);
    }
}

void AddUrlDialog::onOkClicked()
{
    if (m_urlEdit->text().isEmpty()) {
        // Show error
        return;
    }
    accept();
}

void AddUrlDialog::onCancelClicked()
{
    reject();
}

QString AddUrlDialog::extractFilenameFromUrl(const QString &url)
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