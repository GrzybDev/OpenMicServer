#include "wifilistener.hpp"

#include "../openmic.hpp"

WifiListener::WifiListener(QObject *parent)
    : Listener{parent}
{
    connect(this, SIGNAL(StartListener()), SLOT(start()));
    connect(this, SIGNAL(StopListener()), SLOT(stop()));
}

WifiListener::~WifiListener()
{
    emit StopListener();
}

void WifiListener::start()
{
    pollTimer = new QTimer();
    connect(pollTimer, &QTimer::timeout, this, &WifiListener::wifiPoll);
    pollTimer->start(appSettings->Get(SUPPORT_WIFI_BROADCAST_INTERVAL).toUInt());

    QNetworkInterface iface = appSettings->GetNetworkInterface();
    QList<QHostAddress> ifaceAddresses;
    QList<QNetworkAddressEntry> addrEntries = iface.addressEntries();
    QNetworkAddressEntry addrEntry = addrEntries.first();

    foreach (auto addrEntry, addrEntries)
    {
        auto tempAddr = addrEntry.broadcast();

        if (tempAddr != QHostAddress())
            broadcastAddr = tempAddr;
    }

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

    startWebSocket(ifaceAddresses.first(), Server::WIFI);
}

void WifiListener::stop()
{
    if (pollTimer->isActive()) {
        pollTimer->stop();
        pollFuture.cancel();
        webSocket->close();
        broadcastInitialized = false;
    }
}

void WifiListener::wifiPoll()
{
    if (pollFuture.isFinished()) {
        pollFuture = QtConcurrent::run([=]() {
            if (broadcastInitialized)
                broadcastSocket->writeDatagram(broadcastData, broadcastAddr, broadcastPort);
            else
                initWiFi();
        });
    }
}

void WifiListener::initWiFi()
{
    if (!broadcastInitialized) {
        OpenMic* omic = &OpenMic::getInstance();

        QStringList broadcastDataList;
        broadcastDataList.append(QCoreApplication::applicationName());
        broadcastDataList.append(QCoreApplication::applicationVersion());
        broadcastDataList.append(QSysInfo::kernelType());
        broadcastDataList.append(QSysInfo::machineHostName());
        broadcastDataList.append(appSettings->Get(DEVICE_ID).toString());

        QString broadcastDataStr = broadcastDataList.join(BROADCAST_DATA_SEPERATOR);

        broadcastData = broadcastDataStr.toUtf8().toBase64();
        broadcastPort = appSettings->Get(NETWORK_PORT).toUInt();

        broadcastInitialized = true;
        emit omic->changeConnectionStatus(Server::WIFI, true, tr("Waiting for your mobile device at %1:%2").arg(hostAddress.toString()).arg(port));
    }
}
