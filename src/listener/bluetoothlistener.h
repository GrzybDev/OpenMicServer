#ifndef BLUETOOTHLISTENER_H
#define BLUETOOTHLISTENER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "../settings.h"
#include "../ui/DialogDevicePick/devicepickdialog.h"
#include <QtConcurrent/QtConcurrent>
#include <QBluetoothSocket>
#include "listener.h"

class BluetoothListener : public Listener
{
    Q_OBJECT
    Q_INTERFACES(Listener)
public:
    explicit BluetoothListener(QObject *parent = nullptr);
    ~BluetoothListener();

    BluetoothListener(const BluetoothListener&) {}

    static BluetoothListener & getInstance() {
        static BluetoothListener * _instance = nullptr;

        if ( _instance == nullptr )
            _instance = new BluetoothListener();

        return *_instance;
    }

    QBluetoothServer* rfcommServer;

private:
    QFuture<void> pollFuture;
    QTimer* pollTimer = new QTimer(this);

    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothSocket *> clientSockets;
    QMap<QBluetoothSocket *, QString> clientNames;
    bool bluetoothInitialized = false;

    void initBT();
    void checkBluetoothSupport();

private slots:
    void start() override;
    void stop() override;

    void btPoll();
};

#endif // BLUETOOTHLISTENER_H
