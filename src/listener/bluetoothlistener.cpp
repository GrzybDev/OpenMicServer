#include "bluetoothlistener.h"

#include "../openmic.h"

BluetoothListener::BluetoothListener(QObject *parent)
    : Listener{parent}
{
    connect(this, SIGNAL(StartListener()), SLOT(start()));
    connect(this, SIGNAL(StopListener()), SLOT(stop()));
}

BluetoothListener::~BluetoothListener()
{
    emit StopListener();
}

void BluetoothListener::start()
{
    pollTimer = new QTimer();
    connect(pollTimer, &QTimer::timeout, this, &BluetoothListener::btPoll);
    pollTimer->start(appSettings->Get(SUPPORT_BT_CHECK_INTERVAL).toUInt());

    Server* server = &Server::getInstance();

    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol);
    connect(rfcommServer, &QBluetoothServer::newConnection, this, [=](){ server->onNewConnection(Server::BLUETOOTH); });
}

void BluetoothListener::stop()
{
    if (pollTimer->isActive()) {
        pollTimer->stop();
        pollFuture.cancel();
        rfcommServer->close();
        bluetoothInitialized = false;
    }
}

void BluetoothListener::btPoll()
{
    if (pollFuture.isFinished()) {
        pollFuture = QtConcurrent::run([=]() {
            if (bluetoothInitialized)
                checkBluetoothSupport();
            else
                initBT();
        });
    }
}

void BluetoothListener::initBT()
{
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

void BluetoothListener::checkBluetoothSupport()
{
    bool isSupported = Utils::isBluetoothSupported();
    QString status;

    if (isSupported)
        status = tr("Waiting for Bluetooth connection from your phone!");
    else
        status = tr("Bluetooth is either disabled or not available on your device!");

    OpenMic* omic = &OpenMic::getInstance();
    emit omic->changeConnectionStatus(Server::BLUETOOTH, isSupported, status);
}

