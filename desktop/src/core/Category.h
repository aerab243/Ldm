#ifndef CATEGORY_H
#define CATEGORY_H

#include <QObject>
#include <QString>
#include <QDateTime>

class Category
{
public:
    Category();
    Category(int id, const QString &name);

    // Getters
    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getDescription() const { return m_description; }
    QString getDefaultPath() const { return m_defaultPath; }
    QString getColor() const { return m_color; }
    QString getIcon() const { return m_icon; }
    QDateTime getCreatedAt() const { return m_createdAt; }
    QDateTime getUpdatedAt() const { return m_updatedAt; }

    // Setters
    void setId(int id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setDescription(const QString &description) { m_description = description; }
    void setDefaultPath(const QString &path) { m_defaultPath = path; }
    void setColor(const QString &color) { m_color = color; }
    void setIcon(const QString &icon) { m_icon = icon; }
    void setCreatedAt(const QDateTime &time) { m_createdAt = time; }
    void setUpdatedAt(const QDateTime &time) { m_updatedAt = time; }

private:
    int m_id;
    QString m_name;
    QString m_description;
    QString m_defaultPath;
    QString m_color;
    QString m_icon;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // CATEGORY_H