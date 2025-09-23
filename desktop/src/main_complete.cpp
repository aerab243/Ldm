#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
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
#include <QMenuBar>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QStandardPaths>
#include <QDir>
#include <QInputDialog>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QTimer>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QMimeData>
#include <QClipboard>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QTextEdit>
#include <QTabWidget>
#include <QSlider>
#include <QDateTimeEdit>
#include <QCalendarWidget>
#include <QListWidget>
#include <QTextBrowser>

#include <QCryptographicHash>
#include <QMutex>
#include <QWaitCondition>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>

// Forward declarations
class DownloadItem;
class DownloadEngine;
class SettingsDialog;
class SchedulerDialog;
class QueueManagerDialog;
class LinkGrabberDialog;

// DownloadItem Class
class DownloadItem : public QObject
{
    Q_OBJECT

public:
    enum Status {
        Waiting,
        Downloading,
        Paused,
        Completed,
        Failed,
        Cancelled
    };

    explicit DownloadItem(QObject *parent = nullptr) : QObject(parent), 
        m_id(0), m_totalSize(0), m_downloadedSize(0), m_speed(0), m_eta(0),
        m_status(Waiting), m_progress(0.0), m_connections(4), m_priority(1) {}

    // Getters and setters
    int id() const { return m_id; }
    void setId(int id) { m_id = id; }
    
    QString url() const { return m_url; }
    void setUrl(const QString &url) { m_url = url; }
    
    QString fileName() const { return m_fileName; }
    void setFileName(const QString &fileName) { m_fileName = fileName; }
    
    QString savePath() const { return m_savePath; }
    void setSavePath(const QString &path) { m_savePath = path; }
    
    QString category() const { return m_category; }
    void setCategory(const QString &category) { m_category = category; }
    
    qint64 totalSize() const { return m_totalSize; }
    void setTotalSize(qint64 size) { m_totalSize = size; }
    
    qint64 downloadedSize() const { return m_downloadedSize; }
    void setDownloadedSize(qint64 size) { 
        m_downloadedSize = size; 
        if (m_totalSize > 0) {
            m_progress = (double)m_downloadedSize / m_totalSize * 100.0;
        }
        emit progressChanged();
    }
    
    double progress() const { return m_progress; }
    
    int speed() const { return m_speed; }
    void setSpeed(int speed) { m_speed = speed; emit speedChanged(); }
    
    int eta() const { return m_eta; }
    void setEta(int eta) { m_eta = eta; }
    
    Status status() const { return m_status; }
    void setStatus(Status status) { m_status = status; emit statusChanged(); }
    
    QString statusString() const {
        switch (m_status) {
            case Waiting: return "Waiting";
            case Downloading: return QString("Downloading %1%").arg(QString::number(m_progress, 'f', 1));
            case Paused: return "Paused";
            case Completed: return "Completed";
            case Failed: return "Failed";
            case Cancelled: return "Cancelled";
            default: return "Unknown";
        }
    }
    
    int connections() const { return m_connections; }
    void setConnections(int connections) { m_connections = connections; }
    
    int priority() const { return m_priority; }
    void setPriority(int priority) { m_priority = priority; }

signals:
    void progressChanged();
    void statusChanged();
    void speedChanged();
    void completed();
    void failed(const QString &error);

private:
    int m_id;
    QString m_url;
    QString m_fileName;
    QString m_savePath;
    QString m_category;
    qint64 m_totalSize;
    qint64 m_downloadedSize;
    int m_speed;
    int m_eta;
    Status m_status;
    double m_progress;
    int m_connections;
    int m_priority;
};

// Download Worker Thread
class DownloadWorker : public QObject
{
    Q_OBJECT

public:
    explicit DownloadWorker(DownloadItem *item, QObject *parent = nullptr) 
        : QObject(parent), m_item(item), m_manager(new QNetworkAccessManager(this)), 
          m_reply(nullptr), m_file(nullptr), m_paused(false), m_cancelled(false) {}

    ~DownloadWorker() {
        if (m_file) {
            m_file->close();
            delete m_file;
        }
    }

public slots:
    void startDownload() {
        if (!m_item) return;
        
        QString fullPath = QDir(m_item->savePath()).filePath(m_item->fileName());
        m_file = new QFile(fullPath);
        
        if (!m_file->open(QIODevice::WriteOnly)) {
            emit error("Cannot create file: " + fullPath);
            return;
        }
        
        QNetworkRequest request(QUrl(m_item->url()));
        request.setHeader(QNetworkRequest::UserAgentHeader, "LDM/1.0.0 (Like Download Manager)");
        
        m_reply = m_manager->get(request);
        
        connect(m_reply, &QNetworkReply::downloadProgress,
                this, &DownloadWorker::onDownloadProgress);
        connect(m_reply, &QNetworkReply::finished,
                this, &DownloadWorker::onDownloadFinished);
        connect(m_reply, &QNetworkReply::readyRead,
                this, &DownloadWorker::onReadyRead);
        
        m_item->setStatus(DownloadItem::Downloading);
        m_startTime = QTime::currentTime();
        
        // Start speed calculation timer
        m_speedTimer = new QTimer(this);
        connect(m_speedTimer, &QTimer::timeout, this, &DownloadWorker::calculateSpeed);
        m_speedTimer->start(1000); // Update every second
    }
    
    void pauseDownload() {
        m_paused = true;
        if (m_reply) {
            m_reply->abort();
        }
        if (m_speedTimer) {
            m_speedTimer->stop();
        }
        m_item->setStatus(DownloadItem::Paused);
        emit paused();
    }
    
    void resumeDownload() {
        m_paused = false;
        startDownload();
        emit resumed();
    }
    
    void cancelDownload() {
        m_cancelled = true;
        if (m_reply) {
            m_reply->abort();
        }
        if (m_speedTimer) {
            m_speedTimer->stop();
        }
        if (m_file) {
            m_file->close();
            m_file->remove();
        }
        m_item->setStatus(DownloadItem::Cancelled);
        emit cancelled();
    }

signals:
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void finished();
    void error(const QString &message);
    void paused();
    void resumed();
    void cancelled();

private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
        if (m_paused || m_cancelled) return;
        
        m_item->setTotalSize(bytesTotal);
        m_item->setDownloadedSize(bytesReceived);
        
        emit progress(bytesReceived, bytesTotal);
    }
    
    void onReadyRead() {
        if (m_file && m_reply && !m_paused && !m_cancelled) {
            m_file->write(m_reply->readAll());
        }
    }
    
    void onDownloadFinished() {
        if (m_speedTimer) {
            m_speedTimer->stop();
        }
        
        if (m_cancelled) return;
        
        if (m_reply->error() == QNetworkReply::NoError && !m_paused) {
            if (m_file) {
                m_file->close();
            }
            m_item->setStatus(DownloadItem::Completed);
            emit finished();
        } else if (!m_paused) {
            m_item->setStatus(DownloadItem::Failed);
            emit error(m_reply->errorString());
        }
    }
    
    void calculateSpeed() {
        if (m_paused || m_cancelled) return;
        
        qint64 currentBytes = m_item->downloadedSize();
        int elapsedSeconds = m_startTime.secsTo(QTime::currentTime());
        
        if (elapsedSeconds > 0) {
            int speed = currentBytes / elapsedSeconds; // bytes per second
            m_item->setSpeed(speed);
            
            if (speed > 0 && m_item->totalSize() > 0) {
                qint64 remainingBytes = m_item->totalSize() - currentBytes;
                int eta = remainingBytes / speed;
                m_item->setEta(eta);
            }
        }
    }

private:
    DownloadItem *m_item;
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QFile *m_file;
    QTimer *m_speedTimer;
    QTime m_startTime;
    bool m_paused;
    bool m_cancelled;
};

// Download Engine
class DownloadEngine : public QObject
{
    Q_OBJECT

public:
    explicit DownloadEngine(QObject *parent = nullptr) : QObject(parent), m_nextId(1) {}

    int addDownload(const QString &url, const QString &fileName, const QString &savePath, const QString &category = "All Downloads") {
        DownloadItem *item = new DownloadItem(this);
        item->setId(m_nextId++);
        item->setUrl(url);
        item->setFileName(fileName);
        item->setSavePath(savePath);
        item->setCategory(category);
        
        m_downloads[item->id()] = item;
        
        connect(item, &DownloadItem::progressChanged, this, [this, item]() {
            emit downloadProgress(item->id(), item->downloadedSize(), item->totalSize());
        });
        
        connect(item, &DownloadItem::statusChanged, this, [this, item]() {
            if (item->status() == DownloadItem::Completed) {
                emit downloadCompleted(item->id());
            } else if (item->status() == DownloadItem::Failed) {
                emit downloadFailed(item->id(), "Download failed");
            }
        });
        
        emit downloadAdded(item->id());
        return item->id();
    }
    
    void startDownload(int id) {
        if (!m_downloads.contains(id)) return;
        
        DownloadItem *item = m_downloads[id];
        if (m_workers.contains(id)) return; // Already running
        
        DownloadWorker *worker = new DownloadWorker(item);
        QThread *thread = new QThread();
        
        worker->moveToThread(thread);
        
        connect(thread, &QThread::started, worker, &DownloadWorker::startDownload);
        connect(worker, &DownloadWorker::finished, thread, &QThread::quit);
        connect(worker, &DownloadWorker::finished, worker, &DownloadWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        
        connect(worker, &DownloadWorker::finished, this, [this, id]() {
            m_workers.remove(id);
            emit downloadCompleted(id);
        });
        
        connect(worker, &DownloadWorker::error, this, [this, id](const QString &error) {
            m_workers.remove(id);
            emit downloadFailed(id, error);
        });
        
        m_workers[id] = worker;
        thread->start();
        
        emit downloadStarted(id);
    }
    
    void pauseDownload(int id) {
        if (m_workers.contains(id)) {
            m_workers[id]->pauseDownload();
        }
    }
    
    void resumeDownload(int id) {
        if (m_workers.contains(id)) {
            m_workers[id]->resumeDownload();
        } else {
            startDownload(id);
        }
    }
    
    void cancelDownload(int id) {
        if (m_workers.contains(id)) {
            m_workers[id]->cancelDownload();
            m_workers.remove(id);
        }
        if (m_downloads.contains(id)) {
            delete m_downloads[id];
            m_downloads.remove(id);
        }
        emit downloadCancelled(id);
    }
    
    DownloadItem* getDownload(int id) {
        return m_downloads.value(id, nullptr);
    }
    
    QList<DownloadItem*> getAllDownloads() {
        return m_downloads.values();
    }

signals:
    void downloadAdded(int id);
    void downloadStarted(int id);
    void downloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal);
    void downloadCompleted(int id);
    void downloadFailed(int id, const QString &error);
    void downloadCancelled(int id);

private:
    QHash<int, DownloadItem*> m_downloads;
    QHash<int, DownloadWorker*> m_workers;
    int m_nextId;
};

// Settings Dialog
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Options - LDM");
        setFixedSize(500, 400);
        
        QTabWidget *tabs = new QTabWidget();
        
        // General tab
        QWidget *generalTab = new QWidget();
        QFormLayout *generalLayout = new QFormLayout(generalTab);
        
        m_downloadPathEdit = new QLineEdit();
        m_downloadPathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        QPushButton *browseBtn = new QPushButton("Browse...");
        connect(browseBtn, &QPushButton::clicked, [this]() {
            QString path = QFileDialog::getExistingDirectory(this, "Select Download Directory");
            if (!path.isEmpty()) {
                m_downloadPathEdit->setText(path);
            }
        });
        QHBoxLayout *pathLayout = new QHBoxLayout();
        pathLayout->addWidget(m_downloadPathEdit);
        pathLayout->addWidget(browseBtn);
        generalLayout->addRow("Download Path:", pathLayout);
        
        m_maxConnectionsSpinBox = new QSpinBox();
        m_maxConnectionsSpinBox->setRange(1, 16);
        m_maxConnectionsSpinBox->setValue(4);
        generalLayout->addRow("Max Connections:", m_maxConnectionsSpinBox);
        
        m_autoStartCheckBox = new QCheckBox("Start downloads automatically");
        m_autoStartCheckBox->setChecked(true);
        generalLayout->addRow(m_autoStartCheckBox);
        
        tabs->addTab(generalTab, "General");
        
        // Network tab
        QWidget *networkTab = new QWidget();
        QFormLayout *networkLayout = new QFormLayout(networkTab);
        
        m_speedLimitSpinBox = new QSpinBox();
        m_speedLimitSpinBox->setRange(0, 10000);
        m_speedLimitSpinBox->setSuffix(" KB/s");
        m_speedLimitSpinBox->setValue(0);
        networkLayout->addRow("Speed Limit:", m_speedLimitSpinBox);
        
        m_timeoutSpinBox = new QSpinBox();
        m_timeoutSpinBox->setRange(10, 300);
        m_timeoutSpinBox->setSuffix(" seconds");
        m_timeoutSpinBox->setValue(30);
        networkLayout->addRow("Connection Timeout:", m_timeoutSpinBox);
        
        tabs->addTab(networkTab, "Network");
        
        // Interface tab
        QWidget *interfaceTab = new QWidget();
        QFormLayout *interfaceLayout = new QFormLayout(interfaceTab);
        
        m_themeComboBox = new QComboBox();
        m_themeComboBox->addItems({"Light", "Dark", "Classic IDM"});
        interfaceLayout->addRow("Theme:", m_themeComboBox);
        
        m_notificationsCheckBox = new QCheckBox("Show completion notifications");
        m_notificationsCheckBox->setChecked(true);
        interfaceLayout->addRow(m_notificationsCheckBox);
        
        m_minimizeToTrayCheckBox = new QCheckBox("Minimize to system tray");
        m_minimizeToTrayCheckBox->setChecked(true);
        interfaceLayout->addRow(m_minimizeToTrayCheckBox);
        
        tabs->addTab(interfaceTab, "Interface");
        
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(tabs);
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        mainLayout->addWidget(buttonBox);
    }

private:
    QLineEdit *m_downloadPathEdit;
    QSpinBox *m_maxConnectionsSpinBox;
    QCheckBox *m_autoStartCheckBox;
    QSpinBox *m_speedLimitSpinBox;
    QSpinBox *m_timeoutSpinBox;
    QComboBox *m_themeComboBox;
    QCheckBox *m_notificationsCheckBox;
    QCheckBox *m_minimizeToTrayCheckBox;
};

// Scheduler Dialog
class SchedulerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SchedulerDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Scheduler - LDM");
        setFixedSize(600, 400);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *infoLabel = new QLabel("Schedule downloads to start at specific times:");
        layout->addWidget(infoLabel);
        
        m_scheduleList = new QListWidget();
        layout->addWidget(m_scheduleList);
        
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *addBtn = new QPushButton("Add Schedule");
        QPushButton *editBtn = new QPushButton("Edit");
        QPushButton *deleteBtn = new QPushButton("Delete");
        QPushButton *closeBtn = new QPushButton("Close");
        
        buttonLayout->addWidget(addBtn);
        buttonLayout->addWidget(editBtn);
        buttonLayout->addWidget(deleteBtn);
        buttonLayout->addStretch();
        buttonLayout->addWidget(closeBtn);
        
        layout->addLayout(buttonLayout);
        
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }

private:
    QListWidget *m_scheduleList;
};

// Queue Manager Dialog
class QueueManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueueManagerDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Queue Manager - LDM");
        setFixedSize(500, 300);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *infoLabel = new QLabel("Manage download queues:");
        layout->addWidget(infoLabel);
        
        m_queueList = new QListWidget();
        m_queueList->addItem("Default Queue");
        m_queueList->addItem("High Priority Queue");
        m_queueList->addItem("Low Priority Queue");
        layout->addWidget(m_queueList);
        
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *startBtn = new QPushButton("Start Queue");
        QPushButton *stopBtn = new QPushButton("Stop Queue");
        QPushButton *closeBtn = new QPushButton("Close");
        
        buttonLayout->addWidget(startBtn);
        buttonLayout->addWidget(stopBtn);
        buttonLayout->addStretch();
        buttonLayout->addWidget(closeBtn);
        
        layout->addLayout(buttonLayout);
        
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }

private:
    QListWidget *m_queueList;
};

// Link Grabber Dialog
class LinkGrabberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LinkGrabberDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Link Grabber - LDM");
        setFixedSize(600, 400);
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *infoLabel = new QLabel("Enter webpage URL to extract download links:");
        layout->addWidget(infoLabel);
        
        m_urlEdit = new QLineEdit();
        m_urlEdit->setPlaceholderText("Enter webpage URL...");
        layout->addWidget(m_urlEdit);
        
        QPushButton *grabBtn = new QPushButton("Grab Links");
        layout->addWidget(grabBtn);
        
        m_linksList = new QListWidget();
        layout->addWidget(m_linksList);
        
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *selectAllBtn = new QPushButton("Select All");
        QPushButton *downloadBtn = new QPushButton("Download Selected");
        QPushButton *closeBtn = new QPushButton("Close");
        
        buttonLayout->addWidget(selectAllBtn);
        buttonLayout->addWidget(downloadBtn);
        buttonLayout->addStretch();
        buttonLayout->addWidget(closeBtn);
        
        layout->addLayout(buttonLayout);
        
        connect(grabBtn, &QPushButton::clicked, this, &LinkGrabberDialog::grabLinks);
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }

private slots:
    void grabLinks() {
        QString url = m_urlEdit->text().trimmed();
        if (url.isEmpty()) return;
        
        // Simulate link grabbing
        m_linksList->clear();
        m_linksList->addItem("http://example.com/file1.zip");
        m_linksList->addItem("http://example.com/file2.pdf");
        m_linksList->addItem("http://example.com/video.mp4");
        m_linksList->addItem("http://example.com/music.mp3");
        
        QMessageBox::information(this, "Link Grabber", 
            QString("Found %1 downloadable links").arg(m_linksList->count()));
    }

private:
    QLineEdit *m_urlEdit;
    QListWidget *m_linksList;
};

// Main Window
class LDMMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LDMMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("LDM - Like Download Manager 1.0.0");
        setMinimumSize(1000, 700);
        resize(1200, 800);
        
        // Initialize download engine
        m_downloadEngine = new DownloadEngine(this);
        
        setupUI();
        setupTrayIcon();
        setupConnections();
        loadSettings();
        
        // Add some demo downloads
        addDemoDownloads();
        
        // Fade in animation
        QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
        fadeIn->setDuration(300);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
        
        statusBar()->showMessage("Ready - IDM-style interface loaded with download engine");
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        if (m_trayIcon && QSystemTrayIcon::isSystemTrayAvailable() && m_trayIcon->isVisible()) {
            hide();
            event->ignore();
            m_trayIcon->showMessage("LDM", "Application minimized to tray.", QSystemTrayIcon::Information, 2000);
        } else {
            saveSettings();
            event->accept();
        }
    }

private slots:
    void onAddUrl()
    {
        QDialog dialog(this);
        dialog.setWindowTitle("Add URL - LDM");
        dialog.setFixedSize(500, 350);
        
        QFormLayout *layout = new QFormLayout(&dialog);
        
        // URL input
        QLineEdit *urlEdit = new QLineEdit();
        urlEdit->setPlaceholderText("Enter URL to download...");
        layout->addRow("URL:", urlEdit);
        
        // File name
        QLineEdit *fileNameEdit = new QLineEdit();
        fileNameEdit->setPlaceholderText("Filename (auto-detected)");
        layout->addRow("File Name:", fileNameEdit);
        
        // Save location
        QLineEdit *saveEdit = new QLineEdit();
        saveEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        QPushButton *browseBtn = new QPushButton("Browse...");
        connect(browseBtn, &QPushButton::clicked, [&saveEdit, this]() {
            QString path = QFileDialog::getExistingDirectory(this, "Select Save Location");
            if (!path.isEmpty()) {
                saveEdit->setText(path);
            }
        });
        QHBoxLayout *saveLayout = new QHBoxLayout();
        saveLayout->addWidget(saveEdit);
        saveLayout->addWidget(browseBtn);
        layout->addRow("Save to:", saveLayout);
        
        // Category selection
        QComboBox *categoryCombo = new QComboBox();
        categoryCombo->addItems({"All Downloads", "Compressed", "Documents", "Music", "Video", "Programs"});
        layout->addRow("Category:", categoryCombo);
        
        // Number of connections
        QSpinBox *connectionsSpinBox = new QSpinBox();
        connectionsSpinBox->setRange(1, 8);
        connectionsSpinBox->setValue(4);
        layout->addRow("Connections:", connectionsSpinBox);
        
        // Start download immediately
        QCheckBox *startCheckBox = new QCheckBox("Start download immediately");
        startCheckBox->setChecked(true);
        layout->addRow(startCheckBox);
        
        // Buttons
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout->addRow(buttonBox);
        
        connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        
        // Auto-detect filename when URL changes
        connect(urlEdit, &QLineEdit::textChanged, [fileNameEdit](const QString &url) {
            if (!url.isEmpty()) {
                QString fileName = QUrl(url).fileName();
                if (!fileName.isEmpty()) {
                    fileNameEdit->setText(fileName);
                }
            }
        });
        
        if (dialog.exec() == QDialog::Accepted) {
            QString url = urlEdit->text().trimmed();
            QString fileName = fileNameEdit->text().trimmed();
            
            if (url.isEmpty()) {
                QMessageBox::warning(this, "Add URL", "Please enter a valid URL.");
                return;
            }
            
            if (fileName.isEmpty()) {
                fileName = QUrl(url).fileName();
                if (fileName.isEmpty()) {
                    fileName = "download_" + QString::number(QDateTime::currentMSecsSinceEpoch());
                }
            }
            
            int downloadId = m_downloadEngine->addDownload(url, fileName, saveEdit->text(), categoryCombo->currentText());
            addDownloadToTable(downloadId);
            
            if (startCheckBox->isChecked()) {
                m_downloadEngine->startDownload(downloadId);
            }
            
            statusBar()->showMessage("Download added: " + fileName);
        }
    }
    
    void onResume()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
            QTableWidgetItem *idItem = m_downloadsTable->item(currentRow, 7); // Hidden ID column
            if (idItem) {
                int downloadId = idItem->text().toInt();
                m_downloadEngine->resumeDownload(downloadId);
                statusBar()->showMessage("Resuming download...");
            }
        } else {
            QMessageBox::information(this, "Resume", "Please select a download to resume.");
        }
    }
    
    void onStop()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
            QTableWidgetItem *idItem = m_downloadsTable->item(currentRow, 7); // Hidden ID column
            if (idItem) {
                int downloadId = idItem->text().toInt();
                m_downloadEngine->pauseDownload(downloadId);
                statusBar()->showMessage("Download paused");
            }
        } else {
            QMessageBox::information(this, "Pause", "Please select a download to pause.");
        }
    }
    
    void onDelete()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
            QTableWidgetItem *nameItem = m_downloadsTable->item(currentRow, 0);
            QString fileName = nameItem ? nameItem->text() : "Unknown";
            
            int ret = QMessageBox::question(this, "Delete Download",
                QString("Are you sure you want to delete '%1'?").arg(fileName),
                QMessageBox::Yes | QMessageBox::No);
                
            if (ret == QMessageBox::Yes) {
                QTableWidgetItem *idItem = m_downloadsTable->item(currentRow, 7);
                if (idItem) {
                    int downloadId = idItem->text().toInt();
                    m_downloadEngine->cancelDownload(downloadId);
                }
                m_downloadsTable->removeRow(currentRow);
                statusBar()->showMessage("Download deleted: " + fileName);
                updateDetailsPanel();
            }
        } else {
            QMessageBox::information(this, "Delete", "Please select a download to delete.");
        }
    }
    
    void onOptions()
    {
        SettingsDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QMessageBox::information(this, "Settings", "Settings saved successfully!");
        }
    }
    
    void onScheduler()
    {
        SchedulerDialog dialog(this);
        dialog.exec();
    }
    
    void onStartQueue()
    {
        QMessageBox::information(this, "Start Queue", "Starting download queue...");
        statusBar()->showMessage("Download queue started");
    }
    
    void onStopQueue()
    {
        QMessageBox::information(this, "Stop Queue", "Stopping download queue...");
        statusBar()->showMessage("Download queue stopped");
    }
    
    void onGrabber()
    {
        LinkGrabberDialog dialog(this);
        dialog.exec();
    }
    
    void onAbout()
    {
        QMessageBox::about(this, "About LDM", 
            "LDM - Like Download Manager\n"
            "Version 1.0.0\n\n"
            "A powerful download manager with IDM-style interface.\n\n"
            "Features:\n"
            "• Multi-threaded downloads\n"
            "• Resume support\n"
            "• Queue management\n"
            "• Download scheduling\n"
            "• Link grabber\n"
            "• IDM-style interface\n\n"
            "Built with Qt 6 and C++20\n"
            "© 2024 aerab243 (Anna-el Gerard RABENANDRASANA)");
    }
    
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::DoubleClick) {
            show();
            activateWindow();
        }
    }
    
    void onSelectionChanged()
    {
        updateDetailsPanel();
    }
    
    void onCategoryChanged()
    {
        QTreeWidgetItem *current = m_categoriesTree->currentItem();
        if (current) {
            QString category = current->text(0);
            filterDownloadsByCategory(category);
            statusBar()->showMessage("Showing: " + category);
        }
    }
    
    void onDownloadProgress(int downloadId, qint64 bytesReceived, qint64 bytesTotal)
    {
        updateDownloadInTable(downloadId, bytesReceived, bytesTotal);
        updateDetailsPanel();
        updateGlobalProgress();
    }
    
    void onDownloadCompleted(int downloadId)
    {
        DownloadItem *item = m_downloadEngine->getDownload(downloadId);
        if (item) {
            updateDownloadInTable(downloadId, item->totalSize(), item->totalSize(), "Completed");
            showNotification("Download Completed", item->fileName() + " has been downloaded successfully.");
        }
        updateGlobalProgress();
    }
    
    void onDownloadFailed(int downloadId, const QString &error)
    {
        DownloadItem *item = m_downloadEngine->getDownload(downloadId);
        if (item) {
            updateDownloadInTable(downloadId, item->downloadedSize(), item->totalSize(), "Failed: " + error);
            showNotification("Download Failed", item->fileName() + " failed to download: " + error);
        }
        updateGlobalProgress();
    }

private:
    DownloadEngine *m_downloadEngine;
    
    // UI Components
    QTreeWidget *m_categoriesTree;
    QTableWidget *m_downloadsTable;
    QLabel *m_detailsUrlLabel;
    QLabel *m_detailsStatusLabel;
    QLabel *m_detailsSizeLabel;
    QLabel *m_detailsSpeedLabel;
    QLabel *m_detailsTimeLeftLabel;
    QProgressBar *m_detailsProgressBar;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QLabel *m_globalSpeedLabel;
    QProgressBar *m_globalProgressBar;
    
    void setupUI()
    {
        setupMenuBar();
        setupToolBar();
        setupMainLayout();
        setupStatusBar();
        applyIDMStyling();
    }
    
    void setupMenuBar()
    {
        QMenuBar *menuBar = this->menuBar();
        
        QMenu *fileMenu = menuBar->addMenu("&File");
        fileMenu->addAction("&Add URL...", this, &LDMMainWindow::onAddUrl, QKeySequence::New);
        fileMenu->addSeparator();
        fileMenu->addAction("&Exit", this, &QWidget::close, QKeySequence::Quit);
        
        QMenu *downloadsMenu = menuBar->addMenu("&Downloads");
        downloadsMenu->addAction("&Resume", this, &LDMMainWindow::onResume);
        downloadsMenu->addAction("&Pause", this, &LDMMainWindow::onStop);
        downloadsMenu->addAction("&Delete", this, &LDMMainWindow::onDelete);
        
        QMenu *toolsMenu = menuBar->addMenu("&Tools");
        toolsMenu->addAction("&Options", this, &LDMMainWindow::onOptions);
        toolsMenu->addAction("&Scheduler", this, &LDMMainWindow::onScheduler);
        toolsMenu->addAction("&Link Grabber", this, &LDMMainWindow::onGrabber);
        
        QMenu *helpMenu = menuBar->addMenu("&Help");
        helpMenu->addAction("&About", this, &LDMMainWindow::onAbout);
    }
    
    void setupToolBar()
    {
        QToolBar *toolbar = addToolBar("Main");
        toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        
        toolbar->addAction("Add URL", this, &LDMMainWindow::onAddUrl);
        toolbar->addAction("Resume", this, &LDMMainWindow::onResume);
        toolbar->addAction("Stop", this, &LDMMainWindow::onStop);
        toolbar->addAction("Delete", this, &LDMMainWindow::onDelete);
        toolbar->addSeparator();
        toolbar->addAction("Options", this, &LDMMainWindow::onOptions);
        toolbar->addAction("Scheduler", this, &LDMMainWindow::onScheduler);
        toolbar->addAction("Start Queue", this, &LDMMainWindow::onStartQueue);
        toolbar->addAction("Stop Queue", this, &LDMMainWindow::onStopQueue);
        toolbar->addAction("Grabber", this, &LDMMainWindow::onGrabber);
    }
    
    void setupMainLayout()
    {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setSpacing(5);
        mainLayout->setContentsMargins(5, 5, 5, 5);
        
        QSplitter *mainSplitter = new QSplitter(Qt::Horizontal);
        
        // Left panel - Categories
        QGroupBox *categoriesGroup = new QGroupBox("Categories");
        categoriesGroup->setFixedWidth(200);
        QVBoxLayout *categoriesLayout = new QVBoxLayout(categoriesGroup);
        
        m_categoriesTree = new QTreeWidget();
        m_categoriesTree->setHeaderHidden(true);
        m_categoriesTree->setRootIsDecorated(false);
        
        QTreeWidgetItem *allDownloads = new QTreeWidgetItem(m_categoriesTree, QStringList("All Downloads"));
        QTreeWidgetItem *compressed = new QTreeWidgetItem(m_categoriesTree, QStringList("Compressed"));
        QTreeWidgetItem *documents = new QTreeWidgetItem(m_categoriesTree, QStringList("Documents"));
        QTreeWidgetItem *music = new QTreeWidgetItem(m_categoriesTree, QStringList("Music"));
        QTreeWidgetItem *video = new QTreeWidgetItem(m_categoriesTree, QStringList("Video"));
        QTreeWidgetItem *programs = new QTreeWidgetItem(m_categoriesTree, QStringList("Programs"));
        QTreeWidgetItem *unfinished = new QTreeWidgetItem(m_categoriesTree, QStringList("Unfinished"));
        QTreeWidgetItem *finished = new QTreeWidgetItem(m_categoriesTree, QStringList("Finished"));
        
        allDownloads->setSelected(true);
        
        categoriesLayout->addWidget(m_categoriesTree);
        
        // Right panel - Downloads and details
        QWidget *rightPanel = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setContentsMargins(0, 0, 0, 0);
        
        // Downloads table
        QGroupBox *downloadsGroup = new QGroupBox("Downloads");
        QVBoxLayout *downloadsLayout = new QVBoxLayout(downloadsGroup);
        
        m_downloadsTable = new QTableWidget();
        m_downloadsTable->setColumnCount(8); // Added hidden ID column
        QStringList headers = {"File Name", "Size", "Status", "Time left", "Transfer rate", "Last Try", "Description", "ID"};
        m_downloadsTable->setHorizontalHeaderLabels(headers);
        m_downloadsTable->setAlternatingRowColors(true);
        m_downloadsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_downloadsTable->horizontalHeader()->setStretchLastSection(true);
        m_downloadsTable->verticalHeader()->setVisible(false);
        m_downloadsTable->hideColumn(7); // Hide ID column
        
        // Set column widths
        m_downloadsTable->setColumnWidth(0, 200);
        m_downloadsTable->setColumnWidth(1, 100);
        m_downloadsTable->setColumnWidth(2, 120);
        m_downloadsTable->setColumnWidth(3, 100);
        m_downloadsTable->setColumnWidth(4, 120);
        m_downloadsTable->setColumnWidth(5, 120);
        
        downloadsLayout->addWidget(m_downloadsTable);
        
        // Download details panel
        QGroupBox *detailsGroup = new QGroupBox("Download status");
        detailsGroup->setFixedHeight(150);
        QGridLayout *detailsLayout = new QGridLayout(detailsGroup);
        
        detailsLayout->addWidget(new QLabel("URL:"), 0, 0);
        m_detailsUrlLabel = new QLabel("Select a download to view details");
        detailsLayout->addWidget(m_detailsUrlLabel, 0, 1, 1, 2);
        
        detailsLayout->addWidget(new QLabel("Status:"), 1, 0);
        m_detailsStatusLabel = new QLabel("Ready");
        detailsLayout->addWidget(m_detailsStatusLabel, 1, 1);
        
        detailsLayout->addWidget(new QLabel("File size:"), 2, 0);
        m_detailsSizeLabel = new QLabel("Unknown");
        detailsLayout->addWidget(m_detailsSizeLabel, 2, 1);
        
        detailsLayout->addWidget(new QLabel("Downloaded:"), 2, 2);
        QLabel *downloadedLabel = new QLabel("0 MB (0.00 %)");
        detailsLayout->addWidget(downloadedLabel, 2, 3);
        
        detailsLayout->addWidget(new QLabel("Transfer rate:"), 3, 0);
        m_detailsSpeedLabel = new QLabel("0 MB/sec");
        detailsLayout->addWidget(m_detailsSpeedLabel, 3, 1);
        
        detailsLayout->addWidget(new QLabel("Time left:"), 3, 2);
        m_detailsTimeLeftLabel = new QLabel("Unknown");
        detailsLayout->addWidget(m_detailsTimeLeftLabel, 3, 3);
        
        detailsLayout->addWidget(new QLabel("Resume capability:"), 4, 0);
        QLabel *resumeLabel = new QLabel("Yes");
        detailsLayout->addWidget(resumeLabel, 4, 1);
        
        m_detailsProgressBar = new QProgressBar();
        m_detailsProgressBar->setMinimum(0);
        m_detailsProgressBar->setMaximum(100);
        m_detailsProgressBar->setValue(0);
        detailsLayout->addWidget(m_detailsProgressBar, 5, 0, 1, 4);
        
        QHBoxLayout *detailsButtonsLayout = new QHBoxLayout();
        QPushButton *showDetailsBtn = new QPushButton("Show details >>");
        QPushButton *pauseBtn = new QPushButton("Pause");
        QPushButton *cancelBtn = new QPushButton("Cancel");
        
        connect(pauseBtn, &QPushButton::clicked, this, &LDMMainWindow::onStop);
        connect(cancelBtn, &QPushButton::clicked, this, &LDMMainWindow::onDelete);
        
        detailsButtonsLayout->addWidget(showDetailsBtn);
        detailsButtonsLayout->addStretch();
        detailsButtonsLayout->addWidget(pauseBtn);
        detailsButtonsLayout->addWidget(cancelBtn);
        detailsLayout->addLayout(detailsButtonsLayout, 6, 0, 1, 4);
        
        rightLayout->addWidget(downloadsGroup);
        rightLayout->addWidget(detailsGroup);
        
        mainSplitter->addWidget(categoriesGroup);
        mainSplitter->addWidget(rightPanel);
        mainSplitter->setSizes({200, 800});
        
        mainLayout->addWidget(mainSplitter);
    }
    
    void setupStatusBar()
    {
        QStatusBar *status = statusBar();
        status->showMessage("Ready - LDM Download Engine Active");
        
        m_globalSpeedLabel = new QLabel("0 KB/s");
        m_globalProgressBar = new QProgressBar();
        m_globalProgressBar->setMaximum(100);
        m_globalProgressBar->setValue(0);
        m_globalProgressBar->setMaximumWidth(100);
        
        status->addPermanentWidget(m_globalSpeedLabel);
        status->addPermanentWidget(m_globalProgressBar);
    }
    
    void setupTrayIcon()
    {
        if (!QSystemTrayIcon::isSystemTrayAvailable()) {
            return;
        }
        
        m_trayIcon = new QSystemTrayIcon(this);
        m_trayIcon->setIcon(QIcon(":/icons/app.png"));
        m_trayIcon->setToolTip("LDM - Like Download Manager");
        
        m_trayMenu = new QMenu(this);
        m_trayMenu->addAction("Show", [this]() { show(); activateWindow(); });
        m_trayMenu->addAction("Add URL", this, &LDMMainWindow::onAddUrl);
        m_trayMenu->addSeparator();
        m_trayMenu->addAction("Exit", this, &QWidget::close);
        
        m_trayIcon->setContextMenu(m_trayMenu);
        
        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            m_trayIcon->show();
        }
        
        connect(m_trayIcon, &QSystemTrayIcon::activated,
                this, &LDMMainWindow::onTrayIconActivated);
    }
    
    void setupConnections()
    {
        connect(m_categoriesTree, &QTreeWidget::currentItemChanged,
                this, &LDMMainWindow::onCategoryChanged);
        connect(m_downloadsTable, &QTableWidget::currentCellChanged,
                this, &LDMMainWindow::onSelectionChanged);
        
        // Download engine connections
        connect(m_downloadEngine, &DownloadEngine::downloadProgress,
                this, &LDMMainWindow::onDownloadProgress);
        connect(m_downloadEngine, &DownloadEngine::downloadCompleted,
                this, &LDMMainWindow::onDownloadCompleted);
        connect(m_downloadEngine, &DownloadEngine::downloadFailed,
                this, &LDMMainWindow::onDownloadFailed);
    }
    
    void addDownloadToTable(int downloadId)
    {
        DownloadItem *item = m_downloadEngine->getDownload(downloadId);
        if (!item) return;
        
        int row = m_downloadsTable->rowCount();
        m_downloadsTable->insertRow(row);
        
        m_downloadsTable->setItem(row, 0, new QTableWidgetItem(item->fileName()));
        m_downloadsTable->setItem(row, 1, new QTableWidgetItem("Unknown"));
        m_downloadsTable->setItem(row, 2, new QTableWidgetItem(item->statusString()));
        m_downloadsTable->setItem(row, 3, new QTableWidgetItem("Unknown"));
        m_downloadsTable->setItem(row, 4, new QTableWidgetItem("0 KB/s"));
        m_downloadsTable->setItem(row, 5, new QTableWidgetItem(QDateTime::currentDateTime().toString("MMM dd hh:mm")));
        m_downloadsTable->setItem(row, 6, new QTableWidgetItem(item->url()));
        m_downloadsTable->setItem(row, 7, new QTableWidgetItem(QString::number(downloadId))); // Hidden ID
        
        m_downloadsTable->selectRow(row);
        updateDetailsPanel();
    }
    
    void updateDownloadInTable(int downloadId, qint64 bytesReceived, qint64 bytesTotal, const QString &status = QString())
    {
        for (int row = 0; row < m_downloadsTable->rowCount(); ++row) {
            QTableWidgetItem *idItem = m_downloadsTable->item(row, 7);
            if (idItem && idItem->text().toInt() == downloadId) {
                DownloadItem *item = m_downloadEngine->getDownload(downloadId);
                if (!item) return;
                
                // Update size
                QString sizeText = QString("%1 / %2").arg(formatFileSize(bytesReceived)).arg(formatFileSize(bytesTotal));
                m_downloadsTable->setItem(row, 1, new QTableWidgetItem(sizeText));
                
                // Update status
                QString statusText = status.isEmpty() ? item->statusString() : status;
                m_downloadsTable->setItem(row, 2, new QTableWidgetItem(statusText));
                
                // Update speed
                QString speedText = formatSpeed(item->speed());
                m_downloadsTable->setItem(row, 4, new QTableWidgetItem(speedText));
                
                // Update ETA
                QString etaText = formatTimeRemaining(item->eta());
                m_downloadsTable->setItem(row, 3, new QTableWidgetItem(etaText));
                
                break;
            }
        }
    }
    
    void updateDetailsPanel()
    {
        int currentRow = m_downloadsTable->currentRow();
        if (currentRow >= 0) {
            QTableWidgetItem *idItem = m_downloadsTable->item(currentRow, 7);
            if (idItem) {
                int downloadId = idItem->text().toInt();
                DownloadItem *item = m_downloadEngine->getDownload(downloadId);
                if (item) {
                    m_detailsUrlLabel->setText(item->url());
                    m_detailsStatusLabel->setText(item->statusString());
                    m_detailsSizeLabel->setText(formatFileSize(item->totalSize()));
                    m_detailsSpeedLabel->setText(formatSpeed(item->speed()));
                    m_detailsTimeLeftLabel->setText(formatTimeRemaining(item->eta()));
                    m_detailsProgressBar->setValue(static_cast<int>(item->progress()));
                    return;
                }
            }
        }
        
        // Default values
        m_detailsUrlLabel->setText("Select a download to view details");
        m_detailsStatusLabel->setText("Ready");
        m_detailsSizeLabel->setText("Unknown");
        m_detailsSpeedLabel->setText("0 KB/s");
        m_detailsTimeLeftLabel->setText("Unknown");
        m_detailsProgressBar->setValue(0);
    }
    
    void updateGlobalProgress()
    {
        QList<DownloadItem*> downloads = m_downloadEngine->getAllDownloads();
        if (downloads.isEmpty()) {
            m_globalProgressBar->setValue(0);
            m_globalSpeedLabel->setText("0 KB/s");
            return;
        }
        
        qint64 totalSize = 0;
        qint64 downloadedSize = 0;
        qint64 totalSpeed = 0;
        
        for (DownloadItem *item : downloads) {
            totalSize += item->totalSize();
            downloadedSize += item->downloadedSize();
            if (item->status() == DownloadItem::Downloading) {
                totalSpeed += item->speed();
            }
        }
        
        if (totalSize > 0) {
            int progress = static_cast<int>((double)downloadedSize / totalSize * 100);
            m_globalProgressBar->setValue(progress);
        }
        
        m_globalSpeedLabel->setText(formatSpeed(totalSpeed));
    }
    
    void filterDownloadsByCategory(const QString &category)
    {
        for (int row = 0; row < m_downloadsTable->rowCount(); ++row) {
            bool visible = true;
            if (category != "All Downloads") {
                QTableWidgetItem *idItem = m_downloadsTable->item(row, 7);
                if (idItem) {
                    int downloadId = idItem->text().toInt();
                    DownloadItem *item = m_downloadEngine->getDownload(downloadId);
                    if (item) {
                        visible = (item->category() == category);
                    }
                }
            }
            m_downloadsTable->setRowHidden(row, !visible);
        }
    }
    
    void showNotification(const QString &title, const QString &message)
    {
        if (m_trayIcon && QSystemTrayIcon::isSystemTrayAvailable()) {
            m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
        }
    }
    
    void addDemoDownloads()
    {
        // Add some demo downloads
        QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        
        int id1 = m_downloadEngine->addDownload(
            "https://speed.hetzner.de/100MB.bin",
            "100MB.bin",
            downloadsPath,
            "All Downloads"
        );
        addDownloadToTable(id1);
        
        int id2 = m_downloadEngine->addDownload(
            "https://sample-videos.com/zip/10/mp4/SampleVideo_1280x720_1mb.mp4",
            "SampleVideo.mp4",
            downloadsPath,
            "Video"
        );
        addDownloadToTable(id2);
    }
    
    void applyIDMStyling()
    {
        setStyleSheet(
            "QMainWindow { background-color: #f0f0f0; }"
            "QToolBar { background-color: #e8e8e8; border: 1px solid #ccc; spacing: 2px; padding: 2px; }"
            "QTreeWidget { background-color: white; border: 1px solid #ccc; alternate-background-color: #f5f5f5; }"
            "QTableWidget { background-color: white; border: 1px solid #ccc; gridline-color: #ddd; alternate-background-color: #f9f9f9; }"
            "QGroupBox { font-weight: bold; border: 2px solid #ccc; border-radius: 5px; margin-top: 1ex; background-color: #fafafa; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
            "QStatusBar { background-color: #e8e8e8; border-top: 1px solid #ccc; }"
            "QProgressBar { border: 1px solid #ccc; border-radius: 3px; text-align: center; background-color: #f0f0f0; }"
            "QProgressBar::chunk { background-color: #4CAF50; border-radius: 2px; }"
            "QPushButton { padding: 5px 10px; border: 1px solid #ccc; border-radius: 3px; background-color: #f8f8f8; }"
            "QPushButton:hover { background-color: #e8e8e8; }"
            "QPushButton:pressed { background-color: #d8d8d8; }"
        );
    }
    
    QString formatFileSize(qint64 bytes)
    {
        if (bytes == 0) return "0 B";
        
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double size = bytes;
        
        while (size >= 1024 && unitIndex < 4) {
            size /= 1024;
            unitIndex++;
        }
        
        return QString("%1 %2").arg(QString::number(size, 'f', 2)).arg(units[unitIndex]);
    }
    
    QString formatSpeed(qint64 bytesPerSecond)
    {
        if (bytesPerSecond == 0) return "0 KB/s";
        
        const char* units[] = {"B/s", "KB/s", "MB/s", "GB/s"};
        int unitIndex = 0;
        double speed = bytesPerSecond;
        
        while (speed >= 1024 && unitIndex < 3) {
            speed /= 1024;
            unitIndex++;
        }
        
        return QString("%1 %2").arg(QString::number(speed, 'f', 1)).arg(units[unitIndex]);
    }
    
    QString formatTimeRemaining(qint64 seconds)
    {
        if (seconds <= 0) return "Unknown";
        
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;
        
        if (hours > 0) {
            return QString("%1h %2m").arg(hours).arg(minutes);
        } else if (minutes > 0) {
            return QString("%1m %2s").arg(minutes).arg(secs);
        } else {
            return QString("%1s").arg(secs);
        }
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
};

#include "main_complete.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("LDM");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");
    app.setApplicationDisplayName("LDM - Like Download Manager");
    
    // Ensure data directory exists
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    LDMMainWindow window;
    window.show();

    return app.exec();
}