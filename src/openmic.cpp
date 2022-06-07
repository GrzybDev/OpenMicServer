#include "openmic.h"

OpenMic::OpenMic(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();
    server = & Server::getInstance();

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

        if (!isPublic) {
            QFuture<void> usbFuture = QtConcurrent::run([=](){ this->initUSB(); });
        } else {
            emit changeConnectionStatus(connector, true, tr("Currently listening on %1:%2").arg(addr.toString()).arg(port));
        }

        webSockets[connector] = webSocket;
    }
    else
    {
        QString errorString = webSocket->errorString();
        qDebug() << "Failed to start server on" << addr << ":" << port << "(" << errorString << ")";

        emit changeConnectionStatus(connector, false, errorString);
        emit initError(tr("Failed to start %1 listener!\nPlease check if no other program is currently listening on your current communication port!\n\nSystem returned this error: %2").arg(srvName, errorString));
    }
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

void OpenMic::initUSB()
{
    QProcess* adb = new QProcess(this);

    QString program = "adb";
    QStringList arguments;
    arguments << "start-server";

    adb->start(program, arguments);

    if (!adb->waitForStarted())
    {
        QString errorStr = tr("Failed to start ADB!\nEither adb binary is missing, or you may have insufficient permissions to invoke the program.");
        emit changeConnectionStatus(Server::USB, false, errorStr);
        emit initError(errorStr + tr("\n\nYou won't be able to connect via USB! (If you don't intend to use USB, you can disable it in Settings -> Device)"));

        return;
    }

    if (!adb->waitForFinished())
    {
        QString errorStr = tr("ADB took too long to execute, cannot continue USB initialization.");

        emit changeConnectionStatus(Server::USB, false, errorStr);
        emit initError(errorStr + tr("\n\nYou won't be able to connect via USB! (If you don't intend to use USB, you can disable it in Settings -> Device)"));

        return;
    }

    emit changeConnectionStatus(Server::USB, true, tr("Waiting for your mobile device"));
}