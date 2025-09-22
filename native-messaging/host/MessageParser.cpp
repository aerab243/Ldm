#include "MessageParser.h"
#include <QDebug>

MessageParser::MessageParser(QObject *parent)
    : QObject(parent)
{
}

MessageParser::~MessageParser()
{
}

void MessageParser::parseMessage(const QByteArray &data)
{
    m_buffer.append(data);
    processCompleteMessage();
}

void MessageParser::processCompleteMessage()
{
    while (m_buffer.size() >= 4) {
        int messageLength;
        if (!extractMessageLength(m_buffer, messageLength)) {
            break;
        }

        if (m_buffer.size() < 4 + messageLength) {
            break; // Not enough data for the full message
        }

        // Extract the message
        QByteArray messageData = m_buffer.mid(4, messageLength);
        m_buffer = m_buffer.mid(4 + messageLength);

        // Parse JSON
        QJsonDocument doc = QJsonDocument::fromJson(messageData);
        if (doc.isObject()) {
            emit messageReceived(doc.object());
        } else {
            qWarning() << "Invalid JSON message received";
        }
    }
}

bool MessageParser::extractMessageLength(QByteArray &data, int &messageLength)
{
    if (data.size() < 4) {
        return false;
    }

    // Native messaging uses little-endian 32-bit unsigned integer
    messageLength = (static_cast<unsigned char>(data[0])) |
                    (static_cast<unsigned char>(data[1]) << 8) |
                    (static_cast<unsigned char>(data[2]) << 16) |
                    (static_cast<unsigned char>(data[3]) << 24);

    return true;
}