#ifndef OPENMIC_H
#define OPENMIC_H

#include <QObject>
#include <QWebSocketServer>
#include "settings.hpp"
#include "net/server.hpp"
#include <QUdpSocket>
#include <QBluetoothServer>
#include "utils.hpp"
#include "ui/DialogDevicePick/devicepickdialog.hpp"
#include <QtConcurrent/QtConcurrent>
#include "listener/usblistener.hpp"
#include "listener/wifilistener.hpp"
#include "listener/bluetoothlistener.hpp"

class OpenMic : public QObject
{
    Q_OBJECT
public:
    explicit OpenMic(QObject *parent = nullptr);
    OpenMic(const OpenMic&): QObject(nullptr) {}

    static OpenMic & getInstance() {
        static OpenMic * _instance = nullptr;

        if (_instance == nullptr)
            _instance = new OpenMic();

        return *_instance;
    }

    Server* server;
    DevicePickDialog* devicePickDialog = new DevicePickDialog();
    QMap<Server::CONNECTOR, Listener*> listeners;

    void RestartServer();

signals:
    void changeConnectionStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText);
    void showError(QString errorTitle, QString errorText);

private:
    Settings* appSettings;
};

#endif // OPENMIC_H
