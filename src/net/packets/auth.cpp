#include "auth.h"

#include "../server.h"
#include "../../ui/DialogDeviceAuth/deviceauthdialog.h"

PacketAuth::PacketAuth(QObject *parent)
    : QObject{parent}
{

}

QString PacketAuth::Handle(MESSAGE type, QJsonObject data)
{
    switch (type) {
        case AUTH_CODE_VERIFY:
            return handleCodeVerify(data);
        case AUTH_CLIENT_SIDE:
            return handleClientSide(data);
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

QString PacketAuth::handleClientSide(QJsonObject data)
{
    DeviceAuthDialog* dialog = &DeviceAuthDialog::getInstance();

    dialog->generateCode();
    dialog->setModal(true);
    dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);
    dialog->show();

    return "DELAYED_RESPONSE";
}
