#ifndef USBLISTENER_H
#define USBLISTENER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "../settings.h"
#include "../ui/DialogDevicePick/devicepickdialog.h"
#include <QtConcurrent/QtConcurrent>
#include "listener.h"

class USBListener : public Listener
{
    Q_OBJECT
    Q_INTERFACES(Listener)
public:
    explicit USBListener(QObject *parent = nullptr);
    ~USBListener();

    USBListener(const USBListener&): Listener(nullptr) {}

    static USBListener & getInstance() {
        static USBListener * _instance = nullptr;

        if ( _instance == nullptr )
            _instance = new USBListener();

        return *_instance;
    }

signals:
    void updateDeviceList(QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> deviceList);

private:
    QFuture<void> pollFuture;
    QTimer* pollTimer = new QTimer(this);

    QString selectedUSBDevice = "";
    bool usbInitialized = false;

    void initADB();
    void usbPrepare(QString deviceID);

    QStringList getDevices();
    QPair<QList<QPair<QString, Utils::ADB_DEVICE_STATUS>>, QStringList> parseDeviceList(QStringList list);

private slots:
    void start() override;
    void stop() override;

    void usbCheck();
};

#endif // USBLISTENER_H
