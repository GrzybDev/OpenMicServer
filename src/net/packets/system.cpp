#include "system.h"

#include <QCoreApplication>
#include "../handler.h"
#include <QSysInfo>

PacketSystem::PacketSystem(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();
}

QString PacketSystem::Handle(CLIENT_MESSAGE type, QJsonObject data)
{
    switch (type) {
        case CLIENT_SYSTEM_HELLO:
            return handleHello(data);
        default:
            return "";
    }
}

QString PacketSystem::handleHello(QJsonObject data)
{
    QString clientApp = data.value("clientApp").toString();
    QString clientVersion = data.value("clientVersion").toString();

    QJsonObject response;

    qDebug() << "Connected client:" << clientApp << "(Version:" << clientVersion << ")";

    bool clientValid = false;

    if (clientApp == "pl.grzybdev.openmic.client")
    {
        if (clientVersion == QCoreApplication::applicationVersion())
            clientValid = true;
    }
    else if (clientApp != "pl.grzybdev.openmic.client")
        clientValid = true; // Ignore version if not official app

    if (clientValid) {
        response["serverApp"] = QCoreApplication::applicationName();
        response["serverVersion"] = QCoreApplication::applicationVersion();
        response["serverOS"] = QSysInfo::kernelType();
        response["serverName"] = QSysInfo::machineHostName();
        response["serverID"] = appSettings->Get(DEVICE_ID).toString();
    }
    else
    {
        response["error"] = "ERR_VERSION_MISMATCH";
        response["message"] = tr("Version mismatch between client and server!\nIf you're using official apps, please make sure that both client and server have equal version numbers.\n\nCurrent values:\nClient version: %1\nServer version: %2").arg(clientVersion, QCoreApplication::applicationVersion());
    }

    return Handler::GetResponse(SERVER_SYSTEM_HELLO, response);
}
