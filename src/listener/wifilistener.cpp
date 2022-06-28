#include "wifilistener.h"

#include "../openmic.h"

WifiListener::WifiListener(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();

    connect(this, SIGNAL(stopListener()), SLOT(stop()));
}

WifiListener::~WifiListener()
{
    stop();
}

void WifiListener::start()
{
    pollTimer = new QTimer();
    connect(pollTimer, &QTimer::timeout, this, &WifiListener::wifiPoll);
    pollTimer->start(WIFI_BROADCAST_INTERVAL);
}

void WifiListener::stop()
{
    pollTimer->stop();
    pollFuture.cancel();
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

    OpenMic* omic = &OpenMic::getInstance();

    if (omic->StartWebSocketServer(ifaceAddresses.first(), Server::WIFI)) {
        QStringList broadcastDataList;
        broadcastDataList.append(QCoreApplication::applicationName());
        broadcastDataList.append(QCoreApplication::applicationVersion());
        broadcastDataList.append(QSysInfo::kernelType());
        broadcastDataList.append(QSysInfo::machineHostName());
        broadcastDataList.append(appSettings->Get(DEVICE_ID).toString());

        QString broadcastDataStr = broadcastDataList.join(BROADCAST_DATA_SEPERATOR);

        broadcastAddr = addrEntry.broadcast();
        broadcastData = broadcastDataStr.toUtf8().toBase64();
        broadcastPort = appSettings->Get(NETWORK_PORT).toUInt();

        broadcastInitialized = true;
    }
}
