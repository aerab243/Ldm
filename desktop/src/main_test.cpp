#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QDebug>

class SimpleMainWindow : public QMainWindow
{
public:
    SimpleMainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        qDebug() << "Creating SimpleMainWindow...";
        
        setWindowTitle("LDM - Test Window");
        setMinimumSize(800, 600);
        
        // Try to set window icon
        try {
            setWindowIcon(QIcon(":/icons/logo-ldm-opensource.svg"));
            qDebug() << "Window icon set successfully";
        } catch (...) {
            qDebug() << "Failed to set window icon";
        }
        
        // Create simple central widget
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        
        QLabel *titleLabel = new QLabel("LDM - Linux Download Manager", centralWidget);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px;");
        
        QLabel *versionLabel = new QLabel("Version 1.0.0 - Test Build", centralWidget);
        versionLabel->setAlignment(Qt::AlignCenter);
        versionLabel->setStyleSheet("font-size: 16px; color: #666; margin: 10px;");
        
        QLabel *statusLabel = new QLabel("Application started successfully!", centralWidget);
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setStyleSheet("font-size: 14px; color: #008000; margin: 20px;");
        
        layout->addStretch();
        layout->addWidget(titleLabel);
        layout->addWidget(versionLabel);
        layout->addWidget(statusLabel);
        layout->addStretch();
        
        qDebug() << "SimpleMainWindow created successfully";
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Starting LDM test application...";
    
    // Set application properties
    app.setApplicationName("LDM-Test");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");
    
    qDebug() << "Application properties set";
    
    // Try to set application icon
    try {
        app.setWindowIcon(QIcon(":/icons/logo-ldm-opensource.svg"));
        qDebug() << "Application icon set successfully";
    } catch (...) {
        qDebug() << "Failed to set application icon";
    }
    
    qDebug() << "Creating main window...";
    
    SimpleMainWindow window;
    
    qDebug() << "Showing main window...";
    
    window.show();
    
    qDebug() << "Main window shown, starting event loop...";
    
    return app.exec();
}