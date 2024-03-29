#include "handler.hpp"
#include "../utils.hpp"
#include <QJsonDocument>
#include <QMessageBox>

#include "packets/auth.hpp"
#include "packets/stream.hpp"
#include "packets/system.hpp"

Handler::Handler(QObject *parent)
    : QObject{parent}
{
    pSystem = new PacketSystem(this);
    pAuth = new PacketAuth(this);
    pStream = new PacketStream(this);
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
        case AUTH_CLIENT:
            return pAuth->Handle(messageType, msg);
        case STREAM_START:
        case STREAM_VOLUME:
            return pStream->Handle(messageType, msg);
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
        case qConstHash("Auth_Client"):
            return AUTH_CLIENT;
        case qConstHash("Auth_CodeVerify"):
            return AUTH_CODE_VERIFY;
        case qConstHash("Stream_Start"):
            return STREAM_START;
        case qConstHash("Stream_Volume"):
            return STREAM_VOLUME;
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
        case AUTH_CLIENT:
            data["type"] = "Auth_Client";
            break;
        case AUTH_CODE_VERIFY:
            data["type"] = "Auth_CodeVerify";
            break;
        case STREAM_START:
            data["type"] = "Stream_Start";
            break;
        case STREAM_VOLUME:
            data["type"] = "Stream_Volume";
            break;
        default:
            return "";
    }

    QJsonDocument response(data);
    return response.toJson(QJsonDocument::Compact);
}
