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