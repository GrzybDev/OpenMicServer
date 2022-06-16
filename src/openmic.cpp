#include "openmic.h"

#include <QtConcurrent/QtConcurrent>
#include <QBluetoothLocalDevice>
#include "utils.h"

OpenMic::OpenMic(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();
    server = & Server::getInstance();
}

void OpenMic::RestartServer()
{
    StopServers();

    StartServer(false);
    StartServer(true);

    initWiFi();
    initBluetooth();
}

void OpenMic::StartServer(bool isPublic)
{
    QString srvName;
    QHostAddress addr;
    Server::CONNECTOR connector;

    if (isPublic)
    {
        srvName = "OpenMic Server (Wi-Fi)";

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
        connector = Server::WIFI;
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

    wifiTimer->stop();
    btTimer->stop();
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

void OpenMic::initWiFi()
{
    wifiTimer = new QTimer(this);

    QNetworkInterface iface = appSettings->GetNetworkInterface();
    QList<QNetworkAddressEntry> addrEntries = iface.addressEntries();
    QNetworkAddressEntry addrEntry;

    for (int i = 0; i < addrEntries.count(); i++)
    {
        QNetworkAddressEntry addressEntry = addrEntries.at(i);
        QHostAddress address = addressEntry.ip();

        if (!address.isNull() && address.isGlobal()) {
            addrEntry = addressEntry;
            break;
        }
    }

    QStringList broadcastDataList;
    broadcastDataList.append(QCoreApplication::applicationName());
    broadcastDataList.append(QCoreApplication::applicationVersion());
    broadcastDataList.append(QSysInfo::kernelType());
    broadcastDataList.append(QSysInfo::machineHostName());
    broadcastDataList.append(appSettings->Get(DEVICE_ID).toString());

    QString broadcastDataStr = broadcastDataList.join(BROADCAST_DATA_SEPERATOR);

    QHostAddress broadcastAddr = addrEntry.broadcast();
    QByteArray broadcastData = broadcastDataStr.toUtf8().toBase64();
    ushort broadcastPort = appSettings->Get(NETWORK_PORT).toUInt();

    connect(wifiTimer, &QTimer::timeout, this, [=](){ sendBroadcast(broadcastData, broadcastAddr, broadcastPort); });
    wifiTimer->start(WIFI_BROADCAST_INTERVAL);
}

void OpenMic::initBluetooth()
{
    btTimer = new QTimer(this);
    connect(btTimer, &QTimer::timeout, this, &OpenMic::checkBluetoothSupport);
    btTimer->start(BT_CHECK_INTERVAL);

    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer, &QBluetoothServer::newConnection, this, &OpenMic::btClientConnected);

    QBluetoothAddress localAdapter = QBluetoothAddress();
    bool result = rfcommServer->listen(localAdapter);

    if (!result) {
        qWarning() << "Cannot bind OpenMic Server (Bluetooth) to" << localAdapter.toString();
    } else {
        qDebug() << "Successful launch of OpenMic Server (Bluetooth)";
    }

    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, "pl.grzybdev.openmic.server");
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, QCoreApplication::applicationName());
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, QCoreApplication::organizationName());

    static const QLatin1String serviceUuid("6b310fa0-ab0a-4008-8b6a-89b41cb1ccad");
    serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));

    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::PublicBrowseGroup));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             publicBrowse);

    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ProtocolUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ProtocolUuid::Rfcomm))
             << QVariant::fromValue(quint8(rfcommServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);

    serviceInfo.registerService(localAdapter);
}

void OpenMic::checkBluetoothSupport()
{
    bool isSupported = Utils::isBluetoothSupported();
    QString status;

    if (isSupported)
        status = tr("Waiting for Bluetooth connection from your phone!");
    else
        status = tr("Bluetooth is either disabled or not available on your device!");

    emit changeConnectionStatus(Server::BLUETOOTH, isSupported, status);
}

void OpenMic::btClientConnected()
{
    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();

    if (!socket)
        return;

    connect(socket, &QBluetoothSocket::readyRead, this, &OpenMic::btReadSocket);
    connect(socket, &QBluetoothSocket::disconnected, this, QOverload<>::of(&OpenMic::btClientDisconnected));

    clientSockets.append(socket);
    clientNames[socket] = socket->peerName();

    qDebug() << "Bluetooth client connected:" << socket->peerName();
}

void OpenMic::btReadSocket()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());

    if (!socket)
        return;

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();
        qDebug() << "Received message from: " << clientNames[socket] << ", message:" << QString::fromUtf8(line.constData(), line.length());
    }
}

void OpenMic::btClientDisconnected()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());

    if (!socket)
        return;

    qDebug() << "Client disconnected:" << clientNames[socket];

    clientSockets.removeOne(socket);
    clientNames.remove(socket);

    socket->deleteLater();
}

void OpenMic::sendBroadcast(QByteArray broadcastData, QHostAddress broadcastAddr, ushort broadcastPort)
{
    broadcastSocket->writeDatagram(broadcastData, broadcastAddr, broadcastPort);
}
