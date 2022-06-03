#include "auth.h"

#include "../server.h"

PacketAuth::PacketAuth(QObject *parent)
    : QObject{parent}
{

}

QString PacketAuth::Handle(MESSAGE type, QJsonObject data)
{
    switch (type) {
        case AUTH_CODE_VERIFY:
            return handleCodeVerify(data);
        default:
            return "";
    }
}

QString PacketAuth::handleCodeVerify(QJsonObject data)
{
    uint authCode = data.value("authCode").toInt();

    Server* server = &Server::getInstance();
    emit server->handler->onAuthCodeReceived(authCode);

    return "DELAYED_RESPONSE";
}
