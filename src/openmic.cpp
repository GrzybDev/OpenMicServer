#include "openmic.h"

#include <QtConcurrent/QtConcurrent>
#include "utils.h"
#include "ui/DialogDevicePick/devicepickdialog.h"

OpenMic::OpenMic(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();
    server = & Server::getInstance();

    listeners[Server::USB] = &USBListener::getInstance();
    listeners[Server::WIFI] = &WifiListener::getInstance();
    listeners[Server::BLUETOOTH] = &BluetoothListener::getInstance();
}

void OpenMic::RestartServer()
{
    Server* server = & Server::getInstance();

    if (server->connectedClient)
        server->serverDisconnect(SERVER_RESTARTING);

    foreach (auto listener, listeners)
    {
        emit listener->StopListener();
        emit listener->StartListener();
    }
}
