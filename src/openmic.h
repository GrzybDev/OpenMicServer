#ifndef OPENMIC_H
#define OPENMIC_H

#include <QObject>
#include <QWebSocketServer>
#include "settings.h"
#include "net/server.h"
#include <QUdpSocket>
#include <QBluetoothServer>

class OpenMic : public QObject
{
    Q_OBJECT
public:
    explicit OpenMic(QObject *parent = nullptr);
    OpenMic(const OpenMic&) {}

    static OpenMic & getInstance() {
        static OpenMic * _instance = nullptr;

        if (_instance == nullptr)
            _instance = new OpenMic();

        return *_instance;
    }

    void RestartServer();

signals:
    void changeConnectionStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText);
    void initError(QString errorText);

private:
    Settings* appSettings;
    Server* server;

    QTimer* wifiTimer = new QTimer(this);
    QTimer* btTimer = new QTimer(this);

    QMap<Server::CONNECTOR, QWebSocketServer*> webSockets;
    QUdpSocket *broadcastSocket = new QUdpSocket(this);

    QBluetoothServer* rfcommServer = nullptr;
    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothSocket *> clientSockets;
    QMap<QBluetoothSocket *, QString> clientNames;

    void StartServer(bool isPublic);
    void StopServers();

    void initUSB();
    void initWiFi();
    void initBluetooth();

private slots:
    void checkBluetoothSupport();
    void sendBroadcast(QByteArray broadcastData, QHostAddress broadcastAddr, ushort broadcastPort);

    void btClientConnected();
    void btClientDisconnected();
    void btReadSocket();
};

#endif // OPENMIC_H
