#include "ThemeManager.h"
#include <QApplication>
#include <QStyleFactory>

ThemeManager* ThemeManager::instance()
{
    static ThemeManager instance;
    return &instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent), m_currentTheme(Light)
{
    loadStyleSheets();
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        qApp->setStyleSheet(getStyleSheet());
        emit themeChanged(theme);
    }
}

ThemeManager::Theme ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

QString ThemeManager::getStyleSheet() const
{
    return (m_currentTheme == Light) ? m_lightStyleSheet : m_darkStyleSheet;
}

void ThemeManager::loadStyleSheets()
{
    // Light theme - Material Design inspired
    m_lightStyleSheet = R"(
        /* Global */
        * {
            font-family: "Roboto", sans-serif;
        }

        /* Main window */
        QMainWindow {
            background-color: #f5f5f5;
            color: #212121;
        }

        /* Buttons */
        QPushButton {
            background-color: #2196F3;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #1976D2;
        }

        QPushButton:pressed {
            background-color: #0D47A1;
        }

        /* List widgets */
        QListWidget {
            background-color: white;
            border: 1px solid #e0e0e0;
            border-radius: 4px;
        }

        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #f0f0f0;
        }

        QListWidget::item:selected {
            background-color: #E3F2FD;
        }

        /* Progress bars */
        QProgressBar {
            border: 1px solid #e0e0e0;
            border-radius: 4px;
            text-align: center;
        }

        QProgressBar::chunk {
            background-color: #2196F3;
            border-radius: 3px;
        }

        /* Labels */
        QLabel {
            color: #212121;
        }
    )";

    // Dark theme - Material Design inspired
    m_darkStyleSheet = R"(
        /* Global */
        * {
            font-family: "Roboto", sans-serif;
        }

        /* Main window */
        QMainWindow {
            background-color: #303030;
            color: #ffffff;
        }

        /* Buttons */
        QPushButton {
            background-color: #2196F3;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-size: 14px;
        }

        QPushButton:hover {
            background-color: #1976D2;
        }

        QPushButton:pressed {
            background-color: #0D47A1;
        }

        /* List widgets */
        QListWidget {
            background-color: #424242;
            border: 1px solid #616161;
            border-radius: 4px;
            color: #ffffff;
        }

        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #616161;
        }

        QListWidget::item:selected {
            background-color: #1976D2;
        }

        /* Progress bars */
        QProgressBar {
            border: 1px solid #616161;
            border-radius: 4px;
            text-align: center;
            color: #ffffff;
        }

        QProgressBar::chunk {
            background-color: #2196F3;
            border-radius: 3px;
        }

        /* Labels */
        QLabel {
            color: #ffffff;
        }
    )";
}