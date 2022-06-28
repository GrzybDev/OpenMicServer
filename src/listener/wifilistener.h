#ifndef WIFILISTENER_H
#define WIFILISTENER_H

#include <QObject>
#include <QTimer>
#include "../settings.h"
#include <QtConcurrent/QtConcurrent>
#include <QUdpSocket>

class WifiListener : public QObject
{
    Q_OBJECT
public:
    explicit WifiListener(QObject *parent = nullptr);
    ~WifiListener();

    void start();

public slots:
    void stop();

signals:
    void stopListener();

private:
    Settings* appSettings;
    QTimer* pollTimer = new QTimer();

    QUdpSocket *broadcastSocket = new QUdpSocket();

    QFuture<void> pollFuture;

    QHostAddress broadcastAddr;
    QByteArray broadcastData;
    ushort broadcastPort;

    bool broadcastInitialized = false;

    void initWiFi();

private slots:
    void wifiPoll();
};

#endif // WIFILISTENER_H
