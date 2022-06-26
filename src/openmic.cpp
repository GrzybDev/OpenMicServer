#include "openmic.h"

#include <QtConcurrent/QtConcurrent>
#include "utils.h"
#include "ui/DialogDevicePick/devicepickdialog.h"

OpenMic::OpenMic(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();
    server = & Server::getInstance();

    connect(this, &OpenMic::updateDeviceList, devicePickDialog, &DevicePickDialog::updateDeviceList);
}

void OpenMic::RestartServer()
{
    StopServers();

    initUSB();
    initWiFi();
    initBluetooth();
}

bool OpenMic::StartWebSocketServer(QHostAddress hostAddress, Server::CONNECTOR connector)
{
    QWebSocketServer* webSocket = new QWebSocketServer(SERVER_ID, QWebSocketServer::NonSecureMode, this);
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
        emit initError(tr("Failed to start listener (ID: %1)!\nPlease check if no other program is currently listening on your current communication port!\n\nSystem returned this error: %2").arg(QString::number(connector), errorString));
    }

    return listening;
}

void OpenMic::StopServers()
{
    foreach (QWebSocketServer* webSocket, webSockets)
    {
        if (webSocket->isListening())
            webSocket->close();
    }

    if (rfcommServer) {
        rfcommServer->close();
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

    StartWebSocketServer(QHostAddress::LocalHost, Server::USB);

    connect(usbTimer, &QTimer::timeout, this, &OpenMic::usbCheck);
    usbTimer->start(USB_CHECK_INTERVAL);
}

void OpenMic::usbCheck()
{
    QProcess* adb = new QProcess(this);

    QString program = "adb";
    QStringList arguments;
    arguments << "devices";

    adb->start(program, arguments);

    if (!adb->waitForFinished()) {
        QString errorStr = tr("ADB took too long to return list of devices, disabled USB functionality");

        emit changeConnectionStatus(Server::USB, false, errorStr);
        emit initError(errorStr + tr("\n\nYou won't be able to connect via USB! (If you don't intend to use USB, you can disable it in Settings -> Device)"));

        usbTimer->stop();
        return;
    }

    QString output = adb->readAllStandardOutput();
    static QRegularExpression regExp = QRegularExpression("\n|\r\n|\r");
    QStringList devicesStr = output.split(regExp, Qt::SkipEmptyParts);
    devicesStr.removeFirst();

    QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> devices;
    QList<QString> deviceIDs;

    foreach(QString device, devicesStr)
    {
        QStringList deviceInfoList = device.split("\t");
        QPair<QString, Utils::ADB_DEVICE_STATUS> deviceInfo;

        deviceInfo.first = deviceInfoList.first();

        QString statusStr = deviceInfoList.at(1);

        if (statusStr == "device") {
            deviceInfo.second = Utils::ONLINE;
        } else if (statusStr == "unauthorized") {
            deviceInfo.second = Utils::UNAUTHORIZED;
        } else {
            deviceInfo.second = Utils::OFFLINE;
        }

        devices.append(deviceInfo);
        deviceIDs.append(deviceInfo.first);
    }

    emit updateDeviceList(devices);

    if (deviceIDs.contains(selectedUSBDevice))
    {
        // We already set up reverse port to this device, do nothing
        return;
    } else {
        selectedUSBDevice = "";
    }

    if (devices.size() > 1) {
        // More than one device connected, either pick previously selected or show select screen
        QString previousDeviceID = appSettings->Get(USB_AUTO_CONNECT).toString();

        if (deviceIDs.contains(previousDeviceID)) {
            foreach (auto device, devices)
            {
                if (device.first == previousDeviceID)
                {
                    if (device.second != Utils::ONLINE)
                    {
                        qWarning() << "Temporarily skipping" << device.first << "because it's not in valid state!";
                        return;
                    }
                }
            }

            usbPrepare(previousDeviceID);
        } else {
            devicePickDialog->show();
        }
    } else if (devices.size() == 1) {
        auto device = devices.first();

        if (device.second == Utils::ONLINE) {
            usbPrepare(device.first);
        } else {
            qDebug() << "Skipping" << device.first << "because device is in invalid state!";
        }
    }
}

void OpenMic::usbPrepare(QString deviceID)
{
    QProcess* adb = new QProcess(this);

    QString program = "adb";
    QStringList arguments;

    ushort deviceCommunicationPort = appSettings->Get(NETWORK_PORT).toUInt();
    QString reversePort = "tcp:" + QString::number(deviceCommunicationPort);
    arguments << "reverse" << "--remove" << reversePort;

    adb->start(program, arguments);
    adb->waitForFinished();

    arguments.clear();
    arguments << "-s" << deviceID << "reverse" << reversePort << reversePort;

    adb->start(program, arguments);

    if (!adb->waitForFinished() || adb->exitCode() != 0) {
        qCritical() << "Failed to set up port forwarding to USB device! (" << adb->readAllStandardError() << ")";
    } else {
        qDebug() << "Successfully set up port forwarding for device" << deviceID;
        selectedUSBDevice = deviceID;
    }
}

void OpenMic::initWiFi()
{
    wifiTimer = new QTimer(this);

    QNetworkInterface iface = appSettings->GetNetworkInterface();
    QList<QHostAddress> ifaceAddresses;
    QList<QNetworkAddressEntry> addrEntries = iface.addressEntries();
    QNetworkAddressEntry addrEntry = addrEntries.first();

    foreach (QNetworkAddressEntry addressEntry, addrEntries)
    {
        QHostAddress address = addressEntry.ip();

        if (!address.isNull() && address.isGlobal()) {
            ifaceAddresses.append(address);
            addrEntry = addressEntry;
            break;
        }
    }

    if (ifaceAddresses.empty())
    {
        qDebug() << iface.humanReadableName() << "doesn't have any valid IP addressess! Cannot continue.";

        // TODO: Show error message here
        return;
    }

    if (StartWebSocketServer(ifaceAddresses.first(), Server::WIFI)) {
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
}

void OpenMic::initBluetooth()
{
    btTimer = new QTimer(this);
    connect(btTimer, &QTimer::timeout, this, &OpenMic::checkBluetoothSupport);
    btTimer->start(BT_CHECK_INTERVAL);

    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer, &QBluetoothServer::newConnection, this, [=](){ server->onNewConnection(Server::BLUETOOTH); });

    QBluetoothAddress localAdapter = QBluetoothAddress();
    bool result = rfcommServer->listen(localAdapter);

    if (!result) {
        qWarning() << "Cannot bind OpenMic Server (Bluetooth) to" << localAdapter.toString();
        return;
    }

    if (!bluetoothInitialized) {
        serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, SERVER_ID);
        serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, QCoreApplication::applicationName());
        serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, QCoreApplication::organizationName());

        static const QLatin1String serviceUuid(SERVER_GUID);
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
        bluetoothInitialized = true;
    }
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

void OpenMic::sendBroadcast(QByteArray broadcastData, QHostAddress broadcastAddr, ushort broadcastPort)
{
    broadcastSocket->writeDatagram(broadcastData, broadcastAddr, broadcastPort);
}
