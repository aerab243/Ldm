#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Starting LDM simple test...";
    
    // Set application properties
    app.setApplicationName("LDM-Test");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("aerab243");
    
    qDebug() << "Application properties set";
    
    // Try to set application icon
    try {
        app.setWindowIcon(QIcon(":/icons/logo-ldm-opensource.svg"));
        qDebug() << "Icon set successfully";
    } catch (...) {
        qDebug() << "Failed to set icon";
    }
    
    // Create a simple window
    QWidget window;
    window.setWindowTitle("LDM - Simple Test");
    window.resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(&window);
    QLabel *label = new QLabel("LDM Simple Test Window", &window);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    
    qDebug() << "Window created";
    
    window.show();
    
    qDebug() << "Window shown, starting event loop";
    
    return app.exec();
}