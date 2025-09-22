#include "Category.h"

Category::Category()
    : m_id(0)
{
    m_createdAt = QDateTime::currentDateTime();
    m_updatedAt = QDateTime::currentDateTime();
}

Category::Category(int id, const QString &name)
    : m_id(id)
    , m_name(name)
{
    m_createdAt = QDateTime::currentDateTime();
    m_updatedAt = QDateTime::currentDateTime();
}