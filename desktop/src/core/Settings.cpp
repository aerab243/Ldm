#include "Settings.h"

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_type("string")
{
}

Settings::Settings(const QString &key, const QString &value, QObject *parent)
    : QObject(parent)
    , m_key(key)
    , m_value(value)
    , m_type("string")
{
}