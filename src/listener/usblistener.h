#ifndef USBLISTENER_H
#define USBLISTENER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "../settings.h"
#include "../ui/DialogDevicePick/devicepickdialog.h"
#include <QtConcurrent/QtConcurrent>

class USBListener : public QObject
{
    Q_OBJECT
public:
    explicit USBListener(QObject *parent = nullptr);
    ~USBListener();

    void start();

public slots:
    void stop();

signals:
    void updateDeviceList(QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> deviceList);
    void stopListener();

private:
    Settings* appSettings;

    QFuture<void> pollFuture;
    QTimer* pollTimer;

    QString selectedUSBDevice = "";
    bool usbInitialized = false;

    void initADB();
    void usbPrepare(QString deviceID);

private slots:
    void usbCheck();
};

#endif // USBLISTENER_H
