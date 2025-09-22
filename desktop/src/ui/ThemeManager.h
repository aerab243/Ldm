#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme { Light, Dark };

    static ThemeManager* instance();
    void setTheme(Theme theme);
    Theme currentTheme() const;
    QString getStyleSheet() const;

signals:
    void themeChanged(Theme theme);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    Theme m_currentTheme;
    QString m_lightStyleSheet;
    QString m_darkStyleSheet;

    void loadStyleSheets();
};

#endif // THEMEMANAGER_H