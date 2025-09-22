#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>

class Settings : public QObject
{
    Q_OBJECT

public:
    explicit Settings(QObject *parent = nullptr);
    Settings(const QString &key, const QString &value, QObject *parent = nullptr);

    // Getters
    QString getKey() const { return m_key; }
    QString getValue() const { return m_value; }
    QString getType() const { return m_type; }
    QString getCategory() const { return m_category; }
    QString getDescription() const { return m_description; }

    // Setters
    void setKey(const QString &key) { m_key = key; }
    void setValue(const QString &value) { m_value = value; }
    void setType(const QString &type) { m_type = type; }
    void setCategory(const QString &category) { m_category = category; }
    void setDescription(const QString &description) { m_description = description; }

signals:
    void settingsChanged();

private:
    QString m_key;
    QString m_value;
    QString m_type;
    QString m_category;
    QString m_description;
};

#endif // SETTINGS_H