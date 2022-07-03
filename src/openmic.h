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
#include <QtConcurrent/QtConcurrent>
#include "listener/usblistener.h"
#include "listener/wifilistener.h"
#include "listener/bluetoothlistener.h"

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
