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
        case SYSTEM_GOODBYE:
            return handleGoodbye(data);
        default:
            return "";
    }
}

QString PacketSystem::handleHello(QJsonObject data)
{
    QString clientApp = data.value("clientApp").toString();
    QString clientVersion = data.value("clientVersion").toString();
    QString clientID = data.value("clientID").toString();

    Server* server = &Server::getInstance();
    server->connectedClientID = clientID;

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
        QStringList knownIDs = knownIDsRaw.split(PAIRED_DEVICES_SEPERATOR);
        bool needAuth = !knownIDs.contains(clientID);

        response["serverApp"] = QCoreApplication::applicationName();
        response["serverVersion"] = applicationVersion;
        response["serverOS"] = QSysInfo::kernelType();
        response["serverName"] = QSysInfo::machineHostName();
        response["serverID"] = appSettings->Get(DEVICE_ID).toString();
        response["needAuth"] = needAuth;

        if (needAuth) {
            PacketAuth* auth = &PacketAuth::getInstance();
            auth->handleClientSide(QJsonObject());
        }
    }
    else
    {
        response["error"] = VERSION_MISMATCH;
        response["message"] = tr("Version mismatch between client and server!\n\nIf you're using official apps, please make sure that both client and server have equal version numbers.\n\nClient version: %1\nServer version: %2").arg(clientVersion, QCoreApplication::applicationVersion());
        server->serverDisconnect(VERSION_MISMATCH, true);
    }

    return Handler::GetResponse(SYSTEM_HELLO, response);
}

QString PacketSystem::handleGoodbye(QJsonObject data)
{
    int exitCodeInt = data.value("exitCode").toInt();
    EXIT_CODE exitCode = static_cast<EXIT_CODE>(exitCodeInt);

    switch (exitCode)
    {
        case CANCELED_AUTH_CODE_DIALOG:
            dialogDeviceAuth->close();
            break;
        default:
            qDebug() << "Received goodbye message, no additional actions are required!";
            break;
    }

    Server* server = &Server::getInstance();
    server->clientDisconnect();

    return "";
}
