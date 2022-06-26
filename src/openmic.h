#ifndef OPENMIC_H
#define OPENMIC_H

#include <QObject>
#include <QWebSocketServer>
#include "settings.h"
#include "net/server.h"
#include <QUdpSocket>
#include <QBluetoothServer>
#include "utils.h"
#include "ui/DialogDevicePick/devicepickdialog.h"

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

    QMap<Server::CONNECTOR, QWebSocketServer*> webSockets;
    QBluetoothServer* rfcommServer = nullptr;

signals:
    void changeConnectionStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText);
    void initError(QString errorText);
    void updateDeviceList(QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> deviceList);

private:
    Settings* appSettings;
    Server* server;

    QTimer* usbTimer = new QTimer(this);
    QTimer* wifiTimer = new QTimer(this);
    QTimer* btTimer = new QTimer(this);

    DevicePickDialog* devicePickDialog = new DevicePickDialog();
    QString selectedUSBDevice = "";

    QUdpSocket *broadcastSocket = new QUdpSocket(this);

    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothSocket *> clientSockets;
    QMap<QBluetoothSocket *, QString> clientNames;
    bool bluetoothInitialized = false;

    bool StartWebSocketServer(QHostAddress hostAddress, Server::CONNECTOR connector);
    void StopServers();

    void initUSB();
    void initWiFi();
    void initBluetooth();

    void usbCheck();
    void usbPrepare(QString deviceID);

private slots:
    void checkBluetoothSupport();
    void sendBroadcast(QByteArray broadcastData, QHostAddress broadcastAddr, ushort broadcastPort);
};

#endif // OPENMIC_H
