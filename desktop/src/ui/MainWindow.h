#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QSplitter>
#include <QTreeWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QHeaderView>
#include "ui/DownloadListWidget.h"
#include "ui/AddUrlDialog.h"
#include "core/DownloadEngine.h"
#include "core/Database.h"
#include "api/ApiServer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddUrlClicked();
    void onResumeClicked();
    void onStopClicked();
    void onStopAllClicked();
    void onDeleteClicked();
    void onDeleteCompletedClicked();
    void onOptionsClicked();
    void onSchedulerClicked();
    void onStartQueueClicked();
    void onStopQueueClicked();
    void onGrabberClicked();
    void onAboutClicked();
    
    void onCategorySelectionChanged();
    void onDownloadSelectionChanged();
    void onDownloadDoubleClicked(int row, int column);
    
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadCompleted(int downloadId);
    void onDownloadFailed(int downloadId, const QString &error);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    
    // UI Components
    QToolBar *m_mainToolBar;
    QSplitter *m_horizontalSplitter;
    QSplitter *m_verticalSplitter;
    
    // Categories panel
    QTreeWidget *m_categoriesTree;
    
    // Downloads table
    QTableWidget *m_downloadsTable;
    
    // Details panel
    QGroupBox *m_detailsGroup;
    QLabel *m_detailsFileLabel;
    QLabel *m_detailsUrlLabel;
    QLabel *m_detailsSizeLabel;
    QLabel *m_detailsStatusLabel;
    QLabel *m_detailsSpeedLabel;
    QLabel *m_detailsTimeLeftLabel;
    QProgressBar *m_detailsProgressBar;
    QPushButton *m_detailsPauseButton;
    QPushButton *m_detailsCancelButton;
    QPushButton *m_detailsShowDetailsButton;
    
    // System tray
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    
    // Core components
    DownloadEngine *m_downloadEngine;
    Database *m_database;
    ApiServer *m_apiServer;
    
    // Status bar
    QLabel *m_statusLabel;
    QProgressBar *m_globalProgressBar;
    QLabel *m_globalSpeedLabel;

    void setupUI();
    void setupToolBar();
    void setupCategoriesPanel();
    void setupDownloadsTable();
    void setupDetailsPanel();
    void setupStatusBar();
    void setupTrayIcon();
    void setupConnections();
    void setupMenuBar();
    
    void loadDownloads();
    void saveDownloads();
    void updateDownloadsList();
    void updateDetailsPanel();
    void showNotification(const QString &title, const QString &message);
    
    // Table management
    void addDownloadToTable(const struct DownloadItem &item);
    void updateDownloadInTable(int downloadId, qint64 bytesReceived, qint64 bytesTotal, const QString &status = QString());
    void removeDownloadFromTable(int downloadId);
    int findDownloadRowById(int downloadId);
    
    // Categories management
    void populateCategories();
    QString getCurrentCategory();
    void filterDownloadsByCategory(const QString &category);
    
    // UI styling
    void applyIDMStyling();
    
    // Utility functions
    QString formatFileSize(qint64 bytes);
    QString formatSpeed(qint64 bytesPerSecond);
    QString formatTimeRemaining(qint64 seconds);
    QIcon getFileTypeIcon(const QString &filename);
};

#endif // MAINWINDOW_H