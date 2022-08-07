#include "handler.hpp"
#include "../utils.hpp"
#include <QJsonDocument>
#include <QMessageBox>

Handler::Handler(QObject *parent)
    : QObject{parent}
{
    pSystem = new PacketSystem(this);
    pAuth = new PacketAuth(this);
}

QString Handler::HandleCommand(QJsonObject msg)
{
    QJsonValue jsonType = msg.value("type");
    MESSAGE messageType = getMessageType(jsonType.toString());

    switch (messageType) {
        case SYSTEM_HELLO:
        case SYSTEM_GOODBYE:
        case SYSTEM_IS_ALIVE:
            return pSystem->Handle(messageType, msg);
        case AUTH_CODE_VERIFY:
        case AUTH_CLIENT_SIDE:
            return pAuth->Handle(messageType, msg);
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
        case qConstHash("System_IsAlive"):
            return SYSTEM_IS_ALIVE;
        case qConstHash("Auth_CodeVerify"):
            return AUTH_CODE_VERIFY;
        case qConstHash("Auth_ClientSide"):
            return AUTH_CLIENT_SIDE;
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
        case SYSTEM_IS_ALIVE:
            data["type"] = "System_IsAlive";
            break;
        case AUTH_CODE_VERIFY:
            data["type"] = "Auth_CodeVerify";
            break;
        default:
            return "";
    }

    QJsonDocument response(data);
    return response.toJson(QJsonDocument::Compact);
}
