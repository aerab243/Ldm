#include "SettingsDialog.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QDir>

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent)
    : QDialog(parent), m_settings(settings)
{
    setupUI();
    loadSettings();
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), m_settings(nullptr)
{
    setupUI();
    // Don't load settings if m_settings is null
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    setWindowTitle("Settings");
    setModal(true);
    resize(600, 400);

    m_tabWidget = new QTabWidget(this);

    createGeneralTab();
    createNetworkTab();
    createDownloadsTab();
    createConversionTab();

    m_tabWidget->addTab(m_generalTab, "General");
    m_tabWidget->addTab(m_networkTab, "Network");
    m_tabWidget->addTab(m_downloadsTab, "Downloads");
    m_tabWidget->addTab(m_conversionTab, "Conversion");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onSaveClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::onCancelClicked);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::onApplyClicked);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(buttonBox);
}

void SettingsDialog::createGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_generalTab);

    QGroupBox *startupGroup = new QGroupBox("Startup");
    QVBoxLayout *startupLayout = new QVBoxLayout(startupGroup);
    m_autoStartCheck = new QCheckBox("Start automatically on system startup");
    m_minimizeToTrayCheck = new QCheckBox("Minimize to system tray on close");
    startupLayout->addWidget(m_autoStartCheck);
    startupLayout->addWidget(m_minimizeToTrayCheck);

    QGroupBox *appearanceGroup = new QGroupBox("Appearance");
    QFormLayout *appearanceLayout = new QFormLayout(appearanceGroup);
    m_themeCombo = new QComboBox();
    m_themeCombo->addItem("Light", "light");
    m_themeCombo->addItem("Dark", "dark");
    appearanceLayout->addRow("Theme:", m_themeCombo);

    layout->addWidget(startupGroup);
    layout->addWidget(appearanceGroup);
    layout->addStretch();
}

void SettingsDialog::createNetworkTab()
{
    m_networkTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_networkTab);

    QGroupBox *connectionGroup = new QGroupBox("Connection");
    QFormLayout *connectionLayout = new QFormLayout(connectionGroup);
    m_maxConnectionsSpin = new QSpinBox();
    m_maxConnectionsSpin->setRange(1, 32);
    m_timeoutSpin = new QSpinBox();
    m_timeoutSpin->setRange(5, 300);
    m_timeoutSpin->setSuffix(" seconds");
    connectionLayout->addRow("Max connections:", m_maxConnectionsSpin);
    connectionLayout->addRow("Timeout:", m_timeoutSpin);

    QGroupBox *proxyGroup = new QGroupBox("Proxy");
    QFormLayout *proxyLayout = new QFormLayout(proxyGroup);
    m_proxyHostEdit = new QLineEdit();
    m_proxyPortSpin = new QSpinBox();
    m_proxyPortSpin->setRange(1, 65535);
    m_proxyUserEdit = new QLineEdit();
    m_proxyPassEdit = new QLineEdit();
    m_proxyPassEdit->setEchoMode(QLineEdit::Password);
    proxyLayout->addRow("Host:", m_proxyHostEdit);
    proxyLayout->addRow("Port:", m_proxyPortSpin);
    proxyLayout->addRow("Username:", m_proxyUserEdit);
    proxyLayout->addRow("Password:", m_proxyPassEdit);

    layout->addWidget(connectionGroup);
    layout->addWidget(proxyGroup);
    layout->addStretch();
}

void SettingsDialog::createDownloadsTab()
{
    m_downloadsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_downloadsTab);

    QGroupBox *pathsGroup = new QGroupBox("Paths");
    QFormLayout *pathsLayout = new QFormLayout(pathsGroup);
    m_defaultPathEdit = new QLineEdit();
    QPushButton *browseButton = new QPushButton("Browse...");
    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(m_defaultPathEdit);
    pathLayout->addWidget(browseButton);
    pathsLayout->addRow("Default download path:", pathLayout);

    QGroupBox *limitsGroup = new QGroupBox("Limits");
    QFormLayout *limitsLayout = new QFormLayout(limitsGroup);
    m_maxDownloadsSpin = new QSpinBox();
    m_maxDownloadsSpin->setRange(1, 10);
    limitsLayout->addRow("Max simultaneous downloads:", m_maxDownloadsSpin);

    QGroupBox *securityGroup = new QGroupBox("Security");
    QVBoxLayout *securityLayout = new QVBoxLayout(securityGroup);
    m_resumeDownloadsCheck = new QCheckBox("Resume interrupted downloads");
    m_virusScanCheck = new QCheckBox("Scan downloads for viruses");
    securityLayout->addWidget(m_resumeDownloadsCheck);
    securityLayout->addWidget(m_virusScanCheck);

    layout->addWidget(pathsGroup);
    layout->addWidget(limitsGroup);
    layout->addWidget(securityGroup);
    layout->addStretch();

    connect(browseButton, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Download Directory");
        if (!dir.isEmpty()) {
            m_defaultPathEdit->setText(dir);
        }
    });
}

void SettingsDialog::createConversionTab()
{
    m_conversionTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_conversionTab);

    QGroupBox *formatGroup = new QGroupBox("Default Formats");
    QFormLayout *formatLayout = new QFormLayout(formatGroup);
    m_defaultVideoFormatCombo = new QComboBox();
    m_defaultVideoFormatCombo->addItems({"mp4", "avi", "mkv", "mov", "wmv"});
    m_defaultAudioFormatCombo = new QComboBox();
    m_defaultAudioFormatCombo->addItems({"mp3", "aac", "wav", "flac", "ogg"});
    formatLayout->addRow("Default video format:", m_defaultVideoFormatCombo);
    formatLayout->addRow("Default audio format:", m_defaultAudioFormatCombo);

    QGroupBox *optionsGroup = new QGroupBox("Options");
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    m_autoConvertCheck = new QCheckBox("Automatically convert downloads");
    m_extractAudioCheck = new QCheckBox("Extract audio from video files");
    optionsLayout->addWidget(m_autoConvertCheck);
    optionsLayout->addWidget(m_extractAudioCheck);

    layout->addWidget(formatGroup);
    layout->addWidget(optionsGroup);
    layout->addStretch();
}

void SettingsDialog::loadSettings()
{
    if (!m_settings) {
        // Set default values
        m_autoStartCheck->setChecked(false);
        m_minimizeToTrayCheck->setChecked(true);
        m_themeCombo->setCurrentText("Light");
        m_maxConnectionsSpin->setValue(4);
        m_timeoutSpin->setValue(30);
        m_proxyHostEdit->setText("");
        m_proxyPortSpin->setValue(8080);
        m_proxyUserEdit->setText("");
        m_proxyPassEdit->setText("");
        m_defaultPathEdit->setText(QDir::homePath() + "/Downloads");
        m_maxDownloadsSpin->setValue(3);
        m_resumeDownloadsCheck->setChecked(true);
        m_virusScanCheck->setChecked(false);
        m_defaultVideoFormatCombo->setCurrentText("mp4");
        m_defaultAudioFormatCombo->setCurrentText("mp3");
        m_autoConvertCheck->setChecked(false);
        m_extractAudioCheck->setChecked(false);
        return;
    }
    
    // TODO: Load settings from Settings object when API is implemented
}

void SettingsDialog::saveSettings()
{
    if (!m_settings) return;
    
    // TODO: Save settings to Settings object when API is implemented
    // For now, settings are just displayed but not persisted
}

void SettingsDialog::onSaveClicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::onApplyClicked()
{
    saveSettings();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}