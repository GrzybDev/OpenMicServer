#include "openmic.h"

OpenMic::OpenMic(QObject *parent)
    : QObject{parent}
{
    appSettings = new Settings(this);
    server = new Server(this, appSettings);

    RestartServer();
}

void OpenMic::RestartServer()
{
    StopServers();

    StartServer(false);
    StartServer(true);
}

void OpenMic::StartServer(bool isPublic)
{
    QString srvName;
    QHostAddress addr;
    Server::CONNECTOR connector;

    if (isPublic)
    {
        srvName = "OpenMic Server (Wi-Fi)";
        connector = Server::WIFI;

        QNetworkInterface iface = appSettings->GetNetworkInterface();
        QList<QNetworkAddressEntry> addrEntries = iface.addressEntries();
        QList<QHostAddress> ifaceAddresses;

        foreach (QNetworkAddressEntry addressEntry, addrEntries)
        {
            QHostAddress address = addressEntry.ip();

            if (!address.isNull() && address.isGlobal()) {
                ifaceAddresses.append(address);
            }
        }

        if (ifaceAddresses.empty())
        {
            qDebug() << iface.humanReadableName() << "doesn't have any valid IP addressess! Cannot continue.";

            // TODO: Show error message here
            return;
        }

        addr = ifaceAddresses.first();
    } else {
        srvName = "OpenMic Server (USB)";
        addr = QHostAddress::LocalHost;
        connector = Server::USB;
    }

    QWebSocketServer* webSocket = new QWebSocketServer(srvName, QWebSocketServer::NonSecureMode, this);
    quint16 port = appSettings->Get(NETWORK_PORT).toUInt();

    if (webSocket->listen(addr, port))
    {
        qDebug() << "Server is now listening on" << addr << ":" << port;

        connect(webSocket, &QWebSocketServer::newConnection, server, [=]() { server->onNewConnection(webSocket, connector); });
        connect(webSocket, &QWebSocketServer::closed, server, &Server::onClosed);
    }
    else
    {
        qDebug() << "Failed to start server on" << addr << ":" << port << "(" << webSocket->errorString() << ")";
        // TODO: Handle failure
    }

    webSockets.append(webSocket);
}

void OpenMic::StopServers()
{
    foreach (QWebSocketServer* webSocket, webSockets)
    {
        if (webSocket->isListening())
            webSocket->close();
    }

    webSockets.clear();
}
