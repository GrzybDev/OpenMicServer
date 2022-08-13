#include "auth.hpp"

#include "../server.hpp"
#include "../../ui/DialogDeviceAuth/deviceauthdialog.hpp"

PacketAuth::PacketAuth(QObject *parent)
    : Packet{parent}
{
    appSettings = & Settings::getInstance();
}

QString PacketAuth::Handle(MESSAGE type, QJsonObject data)
{
    switch (type) {
        case AUTH_CLIENT:
            return handleAuthClient(data);
        case AUTH_CODE_VERIFY:
            return handleCodeVerify(data);
        default:
            return "";
    }
}

QString PacketAuth::handleAuthClient(QJsonObject data)
{
    Server* serverObj = &Server::getInstance();

    QString knownIDsRaw = appSettings->Get(DEVICE_PAIRED).toString();
    QStringList knownIDs = knownIDsRaw.split(PAIRED_DEVICES_SEPERATOR);
    bool knownDevice = knownIDs.contains(serverObj->connectedClientID);

    QJsonObject response;
    response["authorized"] = knownDevice;

    if (!knownDevice) {
        DeviceAuthDialog* dialog = &DeviceAuthDialog::getInstance();

        dialog->generateCode();
        dialog->setModal(true);
        dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);
        dialog->show();
    }

    return Handler::GetResponse(AUTH_CLIENT, response);
}


QString PacketAuth::handleCodeVerify(QJsonObject data)
{
    uint authCode = data.value("authCode").toInt();

    Server* server = &Server::getInstance();
    emit server->handler->onAuthCodeReceived(authCode);

    return "DELAYED_RESPONSE";
}
