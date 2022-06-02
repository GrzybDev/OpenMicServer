#include "system.h"

#include <QCoreApplication>
#include "../handler.h"
#include <QSysInfo>
#include "../exitcode.h"
#include "../../ui/DialogDeviceAuth/deviceauthdialog.h"

PacketSystem::PacketSystem(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();
}

QString PacketSystem::Handle(MESSAGE type, QJsonObject data)
{
    switch (type) {
        case SYSTEM_HELLO:
            return handleHello(data);
        default:
            return "";
    }
}

QString PacketSystem::handleHello(QJsonObject data)
{
    QString clientApp = data.value("clientApp").toString();
    QString clientVersion = data.value("clientVersion").toString();
    QString clientID = data.value("clientID").toString();

    QJsonObject response;

    qDebug() << "Connected client:" << clientApp << "(Version:" << clientVersion << ")";

    bool clientValid = false;
    QString applicationVersion = QCoreApplication::applicationVersion();

    if (clientApp == "pl.grzybdev.openmic.client")
    {
        if (clientVersion == applicationVersion)
            clientValid = true;
    }
    else if (clientApp != "pl.grzybdev.openmic.client")
        clientValid = true; // Ignore version if not official app

    if (clientValid) {
        QString knownIDsRaw = appSettings->Get(PAIRED_DEVICES).toString();
        QStringList knownIDs = knownIDsRaw.split(";");
        bool needAuth = !knownIDs.contains(clientID);

        response["serverApp"] = QCoreApplication::applicationName();
        response["serverVersion"] = applicationVersion;
        response["serverOS"] = QSysInfo::kernelType();
        response["serverName"] = QSysInfo::machineHostName();
        response["serverID"] = appSettings->Get(DEVICE_ID).toString();
        response["needAuth"] = needAuth;

        if (needAuth) {
            dialogDeviceAuth = new DeviceAuthDialog();
            dialogDeviceAuth->setModal(true);
            dialogDeviceAuth->show();
        }
    }
    else
    {
        response["error"] = "ERR_VERSION_MISMATCH";
        response["message"] = tr("Version mismatch between client and server!\nIf you're using official apps, please make sure that both client and server have equal version numbers.\n\nCurrent values:\nClient version: %1\nServer version: %2").arg(clientVersion, QCoreApplication::applicationVersion());
    }

    return Handler::GetResponse(SYSTEM_HELLO, response);
}
