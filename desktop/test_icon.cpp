#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QPushButton>
#include <QMessageBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QGroupBox>

class IconTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    IconTestWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setupUI();
        setWindowTitle("LDM - Test d'Icône");
        setMinimumSize(400, 300);
        resize(500, 400);
        
        // Tester l'icône de la fenêtre
        QIcon windowIcon(":/icons/logo-ldm-opensource.png");
        if (!windowIcon.isNull()) {
            setWindowIcon(windowIcon);
        }
    }

private slots:
    void testIconResources()
    {
        QString message = "Test des ressources d'icônes:\n\n";
        
        // Test PNG
        QIcon pngIcon(":/icons/logo-ldm-opensource.png");
        message += QString("Logo PNG: %1\n").arg(pngIcon.isNull() ? "❌ ÉCHEC" : "✅ OK");
        
        // Test SVG
        QIcon svgIcon(":/icons/logo-ldm-opensource.svg");
        message += QString("Logo SVG: %1\n").arg(svgIcon.isNull() ? "❌ ÉCHEC" : "✅ OK");
        
        // Test icône app générique
        QIcon appIcon(":/icons/app.png");
        message += QString("Icône App: %1\n").arg(appIcon.isNull() ? "❌ ÉCHEC" : "✅ OK");
        
        // Test autres icônes
        QIcon addIcon(":/icons/add_url.png");
        message += QString("Icône Ajouter: %1\n").arg(addIcon.isNull() ? "❌ ÉCHEC" : "✅ OK");
        
        QIcon pauseIcon(":/icons/pause.png");
        message += QString("Icône Pause: %1\n").arg(pauseIcon.isNull() ? "❌ ÉCHEC" : "✅ OK");
        
        message += "\nTailles disponibles du logo PNG:\n";
        if (!pngIcon.isNull()) {
            QList<QSize> sizes = pngIcon.availableSizes();
            for (const QSize& size : sizes) {
                message += QString("- %1x%2\n").arg(size.width()).arg(size.height());
            }
        }
        
        QMessageBox::information(this, "Résultats du test", message);
    }
    
    void showAbout()
    {
        QMessageBox::about(this, "À propos", 
            "Test d'Icône LDM v1.0\n\n"
            "Ce programme teste l'intégration des icônes\n"
            "dans l'application Linux Download Manager.\n\n"
            "Icônes testées:\n"
            "• Logo principal (PNG/SVG)\n"
            "• Icônes d'interface utilisateur\n"
            "• Icônes de catégories");
    }

private:
    void setupUI()
    {
        QWidget* centralWidget = new QWidget;
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // Titre
        QLabel* titleLabel = new QLabel("Test d'Intégration des Icônes LDM");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        
        // Groupe d'affichage des icônes
        QGroupBox* iconGroup = new QGroupBox("Aperçu des Icônes");
        QHBoxLayout* iconLayout = new QHBoxLayout(iconGroup);
        
        // Afficher le logo principal
        QLabel* logoLabel = new QLabel;
        QPixmap logoPixmap(":/icons/logo-ldm-opensource.png");
        if (!logoPixmap.isNull()) {
            logoLabel->setPixmap(logoPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            logoLabel->setToolTip("Logo LDM (PNG)");
        } else {
            logoLabel->setText("Logo\nIndisponible");
            logoLabel->setStyleSheet("border: 1px solid red; padding: 10px;");
        }
        logoLabel->setAlignment(Qt::AlignCenter);
        
        // Afficher quelques icônes d'interface
        QLabel* addLabel = new QLabel;
        QPixmap addPixmap(":/icons/add_url.png");
        if (!addPixmap.isNull()) {
            addLabel->setPixmap(addPixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            addLabel->setToolTip("Ajouter URL");
        } else {
            addLabel->setText("Add");
            addLabel->setStyleSheet("border: 1px solid red; padding: 5px;");
        }
        addLabel->setAlignment(Qt::AlignCenter);
        
        QLabel* pauseLabel = new QLabel;
        QPixmap pausePixmap(":/icons/pause.png");
        if (!pausePixmap.isNull()) {
            pauseLabel->setPixmap(pausePixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            pauseLabel->setToolTip("Pause");
        } else {
            pauseLabel->setText("Pause");
            pauseLabel->setStyleSheet("border: 1px solid red; padding: 5px;");
        }
        pauseLabel->setAlignment(Qt::AlignCenter);
        
        iconLayout->addWidget(logoLabel);
        iconLayout->addWidget(addLabel);
        iconLayout->addWidget(pauseLabel);
        iconLayout->addStretch();
        
        // Boutons de test
        QHBoxLayout* buttonLayout = new QHBoxLayout;
        
        QPushButton* testButton = new QPushButton("Tester les Ressources");
        QPushButton* aboutButton = new QPushButton("À propos");
        QPushButton* quitButton = new QPushButton("Quitter");
        
        buttonLayout->addWidget(testButton);
        buttonLayout->addWidget(aboutButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(quitButton);
        
        // Instructions
        QLabel* instructionLabel = new QLabel(
            "Ce programme teste l'intégration des icônes dans LDM.\n"
            "• L'icône de la fenêtre devrait être visible dans la barre de titre\n"
            "• Les icônes ci-dessus devraient s'afficher correctement\n"
            "• Cliquez sur 'Tester les Ressources' pour vérifier le chargement"
        );
        instructionLabel->setWordWrap(true);
        instructionLabel->setStyleSheet("padding: 10px; background-color: #f0f0f0; border-radius: 5px; margin: 5px;");
        
        // Assemblage du layout
        mainLayout->addWidget(titleLabel);
        mainLayout->addWidget(iconGroup);
        mainLayout->addWidget(instructionLabel);
        mainLayout->addLayout(buttonLayout);
        
        // Connexions
        connect(testButton, &QPushButton::clicked, this, &IconTestWindow::testIconResources);
        connect(aboutButton, &QPushButton::clicked, this, &IconTestWindow::showAbout);
        connect(quitButton, &QPushButton::clicked, this, &QWidget::close);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Configuration de l'application
    app.setApplicationName("LDM Icon Test");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("LDM");
    
    // Définir l'icône de l'application
    QIcon appIcon(":/icons/logo-ldm-opensource.png");
    if (!appIcon.isNull()) {
        app.setWindowIcon(appIcon);
    }
    
    // Créer et afficher la fenêtre de test
    IconTestWindow window;
    window.show();
    
    return app.exec();
}

#include "test_icon.moc"