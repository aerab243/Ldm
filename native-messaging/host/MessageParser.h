#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

class MessageParser : public QObject
{
    Q_OBJECT

public:
    explicit MessageParser(QObject *parent = nullptr);
    ~MessageParser();

    void parseMessage(const QByteArray &data);

signals:
    void messageReceived(const QJsonObject &message);

private:
    QByteArray m_buffer;

    void processCompleteMessage();
    bool extractMessageLength(QByteArray &data, int &messageLength);
};

#endif // MESSAGEPARSER_H