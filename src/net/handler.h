#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QJsonObject>

#include "packets/system.h"
#include "./servermessage.h"

class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(QObject *parent = nullptr);

    QString HandleCommand(QJsonObject msg);
    static QString GetResponse(SERVER_MESSAGE type, QJsonObject data);

private:
    CLIENT_MESSAGE getMessageType(QString type);

    PacketSystem *pSystem;
};

#endif // HANDLER_H
