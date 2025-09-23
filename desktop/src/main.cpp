#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include <QIcon>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("LDM");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");
    
    // Set application icon using the LDM logo
    app.setWindowIcon(QIcon(":/icons/logo-ldm-opensource.svg"));
    
    // Create splash screen with LDM logo
    QPixmap splashPixmap(":/icons/logo-ldm-opensource.svg");
    splashPixmap = splashPixmap.scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QSplashScreen splash(splashPixmap);
    splash.show();
    
    splash.showMessage("Loading LDM - Linux Download Manager...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();
    
    // Load translations
    QTranslator translator;
    QString locale = QLocale::system().name();
    if (translator.load(QString("ldm_%1").arg(locale))) {
        app.installTranslator(&translator);
    }

    // Load and apply compact responsive LDM theme
    QFile styleFile(":/styles/ldm-compact-responsive.css");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&styleFile);
        app.setStyleSheet(stream.readAll());
        styleFile.close();
    }

    splash.showMessage("Initializing components...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    // Ensure data directory exists
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    splash.showMessage("Starting application...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    // Create and show main window
    MainWindow window;
    
    // Hide splash screen and show main window
    splash.finish(&window);
    window.show();

    return app.exec();
}