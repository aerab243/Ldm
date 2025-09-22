#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("LDM");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");
    // Load translations
    QTranslator translator;
    QString locale = QLocale::system().name();
    if (translator.load(QString("ldm_%1").arg(locale))) {
        app.installTranslator(&translator);
    }


    // Ensure data directory exists
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}