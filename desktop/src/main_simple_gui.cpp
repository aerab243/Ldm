#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QTimer>
#include <QInputDialog>
#include <QSplitter>
#include <QGroupBox>
#include <QRandomGenerator>

class SimpleMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimpleMainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_downloadTable(nullptr)
        , m_urlLineEdit(nullptr)
        , m_addButton(nullptr)
        , m_pauseButton(nullptr)
        , m_resumeButton(nullptr)
        , m_removeButton(nullptr)
        , m_downloadCounter(0)
    {
        setupUI();
        setupMenus();
        setupStatusBar();
        setupTrayIcon();
        
        // Timer pour simuler les mises à jour
        m_updateTimer = new QTimer(this);
        connect(m_updateTimer, &QTimer::timeout, this, &SimpleMainWindow::updateDownloads);
        m_updateTimer->start(1000); // Mise à jour chaque seconde
        
        setWindowTitle("Linux Download Manager (LDM)");
        setMinimumSize(800, 600);
        resize(1000, 700);
    }

private slots:
    void onAddDownload()
    {
        QString url = m_urlLineEdit->text().trimmed();
        if (url.isEmpty()) {
            url = QInputDialog::getText(this, "Ajouter un téléchargement", "URL:");
            if (url.isEmpty()) return;
        }
        
        addDownloadToTable(url);
        m_urlLineEdit->clear();
        statusBar()->showMessage(QString("Téléchargement ajouté: %1").arg(url), 3000);
    }
    
    void onPauseSelected()
    {
        int row = m_downloadTable->currentRow();
        if (row >= 0) {
            QTableWidgetItem* statusItem = m_downloadTable->item(row, 2);
            if (statusItem && statusItem->text() == "En cours") {
                statusItem->setText("En pause");
                statusBar()->showMessage("Téléchargement mis en pause", 2000);
            }
        }
    }
    
    void onResumeSelected()
    {
        int row = m_downloadTable->currentRow();
        if (row >= 0) {
            QTableWidgetItem* statusItem = m_downloadTable->item(row, 2);
            if (statusItem && statusItem->text() == "En pause") {
                statusItem->setText("En cours");
                statusBar()->showMessage("Téléchargement repris", 2000);
            }
        }
    }
    
    void onRemoveSelected()
    {
        int row = m_downloadTable->currentRow();
        if (row >= 0) {
            QString filename = m_downloadTable->item(row, 0)->text();
            int ret = QMessageBox::question(this, "Confirmer", 
                QString("Supprimer le téléchargement '%1' ?").arg(filename));
            if (ret == QMessageBox::Yes) {
                m_downloadTable->removeRow(row);
                statusBar()->showMessage("Téléchargement supprimé", 2000);
            }
        }
    }
    
    void onOpenFolder()
    {
        QString path = QFileDialog::getExistingDirectory(this, "Choisir le dossier de téléchargement");
        if (!path.isEmpty()) {
            statusBar()->showMessage(QString("Dossier: %1").arg(path), 3000);
        }
    }
    
    void onSettings()
    {
        QMessageBox::information(this, "Paramètres", "Fenêtre de paramètres (à implémenter)");
    }
    
    void onAbout()
    {
        QMessageBox::about(this, "À propos", 
            "Linux Download Manager (LDM) v1.0.0\n\n"
            "Gestionnaire de téléchargements moderne pour Linux\n"
            "Développé par Anna-el Gerard RABENANDRASANA\n\n"
            "Interface graphique simplifiée fonctionnelle.");
    }
    
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::DoubleClick) {
            if (isVisible()) {
                hide();
            } else {
                show();
                raise();
                activateWindow();
            }
        }
    }
    
    void updateDownloads()
    {
        // Simuler la progression des téléchargements
        for (int row = 0; row < m_downloadTable->rowCount(); ++row) {
            QTableWidgetItem* statusItem = m_downloadTable->item(row, 2);
            QProgressBar* progressBar = qobject_cast<QProgressBar*>(m_downloadTable->cellWidget(row, 3));
            
            if (statusItem && statusItem->text() == "En cours" && progressBar) {
                int currentValue = progressBar->value();
                if (currentValue < 100) {
                    progressBar->setValue(currentValue + 1);
                    
                    // Simuler la vitesse
                    QTableWidgetItem* speedItem = m_downloadTable->item(row, 4);
                    if (speedItem) {
                        int speed = 500 + (QRandomGenerator::global()->bounded(1000)); // 500-1500 KB/s
                        speedItem->setText(QString("%1 KB/s").arg(speed));
                    }
                    
                    if (progressBar->value() == 100) {
                        statusItem->setText("Terminé");
                        QTableWidgetItem* speedItem = m_downloadTable->item(row, 4);
                        if (speedItem) speedItem->setText("--");
                    }
                }
            }
        }
    }

private:
    void setupUI()
    {
        QWidget* centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // Zone d'ajout d'URL
        QGroupBox* addGroup = new QGroupBox("Ajouter un téléchargement");
        QHBoxLayout* addLayout = new QHBoxLayout(addGroup);
        
        m_urlLineEdit = new QLineEdit;
        m_urlLineEdit->setPlaceholderText("Entrez l'URL à télécharger...");
        m_addButton = new QPushButton("Ajouter");
        
        addLayout->addWidget(new QLabel("URL:"));
        addLayout->addWidget(m_urlLineEdit);
        addLayout->addWidget(m_addButton);
        
        // Boutons de contrôle
        QHBoxLayout* controlLayout = new QHBoxLayout;
        m_pauseButton = new QPushButton("Pause");
        m_resumeButton = new QPushButton("Reprendre");
        m_removeButton = new QPushButton("Supprimer");
        QPushButton* folderButton = new QPushButton("Ouvrir dossier");
        
        controlLayout->addWidget(m_pauseButton);
        controlLayout->addWidget(m_resumeButton);
        controlLayout->addWidget(m_removeButton);
        controlLayout->addWidget(folderButton);
        controlLayout->addStretch();
        
        // Table des téléchargements
        m_downloadTable = new QTableWidget;
        m_downloadTable->setColumnCount(5);
        QStringList headers;
        headers << "Fichier" << "Taille" << "État" << "Progression" << "Vitesse";
        m_downloadTable->setHorizontalHeaderLabels(headers);
        
        // Configuration de la table
        m_downloadTable->horizontalHeader()->setStretchLastSection(true);
        m_downloadTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_downloadTable->setAlternatingRowColors(true);
        m_downloadTable->setSortingEnabled(true);
        
        // Tailles des colonnes
        m_downloadTable->setColumnWidth(0, 200);
        m_downloadTable->setColumnWidth(1, 100);
        m_downloadTable->setColumnWidth(2, 100);
        m_downloadTable->setColumnWidth(3, 150);
        m_downloadTable->setColumnWidth(4, 100);
        
        mainLayout->addWidget(addGroup);
        mainLayout->addLayout(controlLayout);
        mainLayout->addWidget(m_downloadTable);
        
        // Connexions
        connect(m_addButton, &QPushButton::clicked, this, &SimpleMainWindow::onAddDownload);
        connect(m_urlLineEdit, &QLineEdit::returnPressed, this, &SimpleMainWindow::onAddDownload);
        connect(m_pauseButton, &QPushButton::clicked, this, &SimpleMainWindow::onPauseSelected);
        connect(m_resumeButton, &QPushButton::clicked, this, &SimpleMainWindow::onResumeSelected);
        connect(m_removeButton, &QPushButton::clicked, this, &SimpleMainWindow::onRemoveSelected);
        connect(folderButton, &QPushButton::clicked, this, &SimpleMainWindow::onOpenFolder);
    }
    
    void setupMenus()
    {
        // Menu Fichier
        QMenu* fileMenu = menuBar()->addMenu("&Fichier");
        fileMenu->addAction("&Nouveau téléchargement", QKeySequence("Ctrl+N"), this, &SimpleMainWindow::onAddDownload);
        fileMenu->addSeparator();
        fileMenu->addAction("&Quitter", QKeySequence("Ctrl+Q"), this, &QWidget::close);
        
        // Menu Téléchargements
        QMenu* downloadsMenu = menuBar()->addMenu("&Téléchargements");
        downloadsMenu->addAction("&Reprendre", QKeySequence("F9"), this, &SimpleMainWindow::onResumeSelected);
        downloadsMenu->addAction("&Pause", QKeySequence("F5"), this, &SimpleMainWindow::onPauseSelected);
        downloadsMenu->addAction("&Supprimer", QKeySequence("Delete"), this, &SimpleMainWindow::onRemoveSelected);
        downloadsMenu->addSeparator();
        downloadsMenu->addAction("&Ouvrir dossier", this, &SimpleMainWindow::onOpenFolder);
        
        // Menu Outils
        QMenu* toolsMenu = menuBar()->addMenu("&Outils");
        toolsMenu->addAction("&Paramètres", QKeySequence("Ctrl+P"), this, &SimpleMainWindow::onSettings);
        
        // Menu Aide
        QMenu* helpMenu = menuBar()->addMenu("&Aide");
        helpMenu->addAction("À &propos", this, &SimpleMainWindow::onAbout);
    }
    
    void setupStatusBar()
    {
        statusBar()->showMessage("LDM prêt - Ajoutez des téléchargements pour commencer");
    }
    
    void setupTrayIcon()
    {
        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            m_trayIcon = new QSystemTrayIcon(this);
            m_trayIcon->setIcon(QIcon(":/icons/app.png"));
            m_trayIcon->setToolTip("Linux Download Manager");
            
            QMenu* trayMenu = new QMenu(this);
            trayMenu->addAction("Afficher", this, &QWidget::show);
            trayMenu->addAction("Masquer", this, &QWidget::hide);
            trayMenu->addSeparator();
            trayMenu->addAction("Quitter", this, &QWidget::close);
            
            m_trayIcon->setContextMenu(trayMenu);
            connect(m_trayIcon, &QSystemTrayIcon::activated, this, &SimpleMainWindow::onTrayActivated);
            
            m_trayIcon->show();
        }
    }
    
    void addDownloadToTable(const QString& url)
    {
        int row = m_downloadTable->rowCount();
        m_downloadTable->insertRow(row);
        
        // Extraire le nom de fichier de l'URL
        QString filename = url.split('/').last();
        if (filename.isEmpty()) {
            filename = QString("téléchargement_%1").arg(++m_downloadCounter);
        }
        
        // Simuler une taille de fichier
        int sizeKB = 1000 + (QRandomGenerator::global()->bounded(50000)); // 1-51 MB
        QString sizeStr;
        if (sizeKB > 1024) {
            sizeStr = QString("%1 MB").arg(sizeKB / 1024.0, 0, 'f', 1);
        } else {
            sizeStr = QString("%1 KB").arg(sizeKB);
        }
        
        // Ajouter les éléments
        m_downloadTable->setItem(row, 0, new QTableWidgetItem(filename));
        m_downloadTable->setItem(row, 1, new QTableWidgetItem(sizeStr));
        m_downloadTable->setItem(row, 2, new QTableWidgetItem("En cours"));
        m_downloadTable->setItem(row, 4, new QTableWidgetItem("0 KB/s"));
        
        // Ajouter la barre de progression
        QProgressBar* progressBar = new QProgressBar;
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        m_downloadTable->setCellWidget(row, 3, progressBar);
        
        // Sélectionner la nouvelle ligne
        m_downloadTable->selectRow(row);
    }

private:
    QTableWidget* m_downloadTable;
    QLineEdit* m_urlLineEdit;
    QPushButton* m_addButton;
    QPushButton* m_pauseButton;
    QPushButton* m_resumeButton;
    QPushButton* m_removeButton;
    QSystemTrayIcon* m_trayIcon;
    QTimer* m_updateTimer;
    int m_downloadCounter;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Configuration de l'application
    app.setApplicationName("Linux Download Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("LDM");
    app.setOrganizationDomain("github.com/aerab243/ldm");
    
    // Créer et afficher la fenêtre principale
    SimpleMainWindow window;
    window.show();
    
    return app.exec();
}

#include "main_simple_gui.moc"