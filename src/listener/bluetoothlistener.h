#ifndef BLUETOOTHLISTENER_H
#define BLUETOOTHLISTENER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "../settings.h"
#include "../ui/DialogDevicePick/devicepickdialog.h"
#include <QtConcurrent/QtConcurrent>
#include <QBluetoothSocket>

class BluetoothListener : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothListener(QObject *parent = nullptr);
    ~BluetoothListener();

    void start();

public slots:
    void stop();

signals:
    void stopListener();

private:
    Settings* appSettings;

    QFuture<void> pollFuture;
    QTimer* pollTimer;

    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothSocket *> clientSockets;
    QMap<QBluetoothSocket *, QString> clientNames;
    bool bluetoothInitialized = false;

    void initBT();
    void checkBluetoothSupport();

private slots:
    void btPoll();
};

#endif // BLUETOOTHLISTENER_H
