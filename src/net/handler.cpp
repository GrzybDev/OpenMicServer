#include "handler.h"
#include "../utils.h"
#include <QJsonDocument>

Handler::Handler(QObject *parent)
    : QObject{parent}
{
    cSystem = new PacketClientSystem(this);
}

QString Handler::HandleCommand(QJsonObject msg)
{
    QJsonValue jsonType = msg.value("type");
    CLIENT_MESSAGE messageType = getMessageType(jsonType.toString());

    switch (messageType) {
        case CLIENT_SYSTEM_HELLO:
            return cSystem->Handle(messageType, msg);
        default:
            return "";
    }
}

CLIENT_MESSAGE Handler::getMessageType(QString type)
{
    switch (qt_hash(type)) {
        case qConstHash("System_Hello"):
            return CLIENT_SYSTEM_HELLO;
        default:
            return CLIENT_UNKNOWN;
    }
}

QString Handler::GetResponse(SERVER_MESSAGE type, QJsonObject data)
{
    switch (type) {
        case SERVER_SYSTEM_HELLO:
            data["type"] = "System_Hello";
            break;
    }

    QJsonDocument response(data);
    return response.toJson(QJsonDocument::Compact);
}
