#include "openmic.h"

#include <QtConcurrent/QtConcurrent>
#include "utils.h"
#include "ui/DialogDevicePick/devicepickdialog.h"

OpenMic::OpenMic(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();
    server = & Server::getInstance();
}

void OpenMic::RestartServer()
{
    StopServers();

    usbListener = new USBListener();
    usbListener->start();

    wifiListener = new WifiListener();
    wifiListener->start();

    btListener = new BluetoothListener();
    btListener->start();
}

bool OpenMic::StartWebSocketServer(QHostAddress hostAddress, Server::CONNECTOR connector)
{
    QWebSocketServer* webSocket = new QWebSocketServer(SERVER_ID, QWebSocketServer::NonSecureMode);
    quint16 port = appSettings->Get(NETWORK_PORT).toUInt();

    bool listening = webSocket->listen(hostAddress, port);

    if (listening)
    {
        qDebug() << "Server is now listening on" << hostAddress << ":" << port;

        connect(webSocket, &QWebSocketServer::newConnection, server, [=]() { server->onNewConnection(connector); });
        connect(webSocket, &QWebSocketServer::closed, server, &Server::onClosed);

        webSockets[connector] = webSocket;

        emit changeConnectionStatus(connector, true, tr("Waiting for your mobile device at %1:%2").arg(hostAddress.toString()).arg(port));
    }
    else
    {
        QString errorString = webSocket->errorString();
        qDebug() << "Failed to start server on" << hostAddress << ":" << port << "(" << errorString << ")";

        emit changeConnectionStatus(connector, false, errorString);
        emit showError(tr("Listener error"), tr("Failed to start listener (ID: %1)!\nPlease check if no other program is currently listening on your current communication port!\n\nSystem returned this error: %2").arg(QString::number(connector), errorString));
    }

    return listening;
}

void OpenMic::StopServers()
{
    foreach (QWebSocketServer* webSocket, webSockets)
    {
        if (webSocket->isListening())
        {
            webSocket->close();
            webSocket->deleteLater();
        }
    }

    if (rfcommServer) {
        rfcommServer->close();
        rfcommServer->deleteLater();
    }

    webSockets.clear();
}
