#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include "core/Settings.h"



class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(Settings *settings, QWidget *parent = nullptr);
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

signals:
    void settingsChanged();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onApplyClicked();

private:
    Settings *m_settings;
    QTabWidget *m_tabWidget;

    // General tab
    QWidget *m_generalTab;
    QCheckBox *m_autoStartCheck;
    QCheckBox *m_minimizeToTrayCheck;
    QComboBox *m_themeCombo;

    // Network tab
    QWidget *m_networkTab;
    QSpinBox *m_maxConnectionsSpin;
    QSpinBox *m_timeoutSpin;
    QLineEdit *m_proxyHostEdit;
    QSpinBox *m_proxyPortSpin;
    QLineEdit *m_proxyUserEdit;
    QLineEdit *m_proxyPassEdit;

    // Downloads tab
    QWidget *m_downloadsTab;
    QLineEdit *m_defaultPathEdit;
    QSpinBox *m_maxDownloadsSpin;
    QCheckBox *m_resumeDownloadsCheck;
    QCheckBox *m_virusScanCheck;

    // Conversion tab
    QWidget *m_conversionTab;
    QComboBox *m_defaultVideoFormatCombo;
    QComboBox *m_defaultAudioFormatCombo;
    QCheckBox *m_autoConvertCheck;
    QCheckBox *m_extractAudioCheck;

    // Buttons
    QPushButton *m_saveButton;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

    void setupUI();
    void loadSettings();
    void saveSettings();
    void createGeneralTab();
    void createNetworkTab();
    void createDownloadsTab();
    void createConversionTab();
};

#endif // SETTINGSDIALOG_H