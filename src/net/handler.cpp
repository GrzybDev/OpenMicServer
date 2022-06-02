#include "handler.h"
#include "../utils.h"
#include <QJsonDocument>

Handler::Handler(QObject *parent)
    : QObject{parent}
{
    pSystem = new PacketSystem(this);
}

QString Handler::HandleCommand(QJsonObject msg)
{
    QJsonValue jsonType = msg.value("type");
    MESSAGE messageType = getMessageType(jsonType.toString());

    switch (messageType) {
        case SYSTEM_HELLO:
        case SYSTEM_GOODBYE:
            return pSystem->Handle(messageType, msg);
        default:
            return "";
    }
}

MESSAGE Handler::getMessageType(QString type)
{
    switch (qt_hash(type)) {
        case qConstHash("System_Hello"):
            return SYSTEM_HELLO;
        case qConstHash("System_Goodbye"):
            return SYSTEM_GOODBYE;
        default:
            return UNKNOWN;
    }
}

QString Handler::GetResponse(MESSAGE type, QJsonObject data)
{
    switch (type) {
        case SYSTEM_HELLO:
            data["type"] = "System_Hello";
            break;
        case SYSTEM_GOODBYE:
            data["type"] = "System_Goodbye";
            break;
        default:
            break;
    }

    QJsonDocument response(data);
    return response.toJson(QJsonDocument::Compact);
}
