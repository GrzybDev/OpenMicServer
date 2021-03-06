#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QJsonObject>

#include "packets/system.hpp"
#include "packets/auth.hpp"
#include "./message.hpp"

class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(QObject *parent = nullptr);

    QString HandleCommand(QJsonObject msg);
    static QString GetResponse(MESSAGE type, QJsonObject data);

signals:
    void onAuthCodeReceived(int authCode);

private:
    MESSAGE getMessageType(QString type);

    PacketSystem *pSystem;
    PacketAuth *pAuth;
};

#endif // HANDLER_H
