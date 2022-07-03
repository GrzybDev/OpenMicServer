#ifndef WIFILISTENER_H
#define WIFILISTENER_H

#include <QObject>
#include <QTimer>
#include "../settings.h"
#include <QtConcurrent/QtConcurrent>
#include <QUdpSocket>
#include "listener.h"

class WifiListener : public Listener
{
    Q_OBJECT
    Q_INTERFACES(Listener)
public:
    explicit WifiListener(QObject *parent = nullptr);
    ~WifiListener();

    WifiListener(const WifiListener&): Listener(nullptr) {}

    static WifiListener & getInstance() {
        static WifiListener * _instance = nullptr;

        if ( _instance == nullptr )
            _instance = new WifiListener();

        return *_instance;
    }

private:
    QTimer* pollTimer = new QTimer();

    QUdpSocket *broadcastSocket = new QUdpSocket();

    QFuture<void> pollFuture;

    QHostAddress broadcastAddr;
    QByteArray broadcastData;
    ushort broadcastPort;

    bool broadcastInitialized = false;

    void initWiFi();

private slots:
    void start() override;
    void stop() override;

    void wifiPoll();
};

#endif // WIFILISTENER_H
