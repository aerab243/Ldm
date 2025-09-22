#ifndef ADDURLDIALOG_H
#define ADDURLDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include "core/Category.h"
#include "core/Database.h"

class AddUrlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUrlDialog(Database *database, QWidget *parent = nullptr);
    ~AddUrlDialog();

    QString getUrl() const;
    QString getFileName() const;
    int getCategoryId() const;

private slots:
    void onBrowseClicked();
    void onOkClicked();
    void onCancelClicked();

private:
    Database *m_database;
    QLineEdit *m_urlEdit;
    QLineEdit *m_fileNameEdit;
    QComboBox *m_categoryCombo;
    QPushButton *m_browseButton;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

    void setupUI();
    void loadCategories();
    QString extractFilenameFromUrl(const QString &url);
};

#endif // ADDURLDIALOG_H