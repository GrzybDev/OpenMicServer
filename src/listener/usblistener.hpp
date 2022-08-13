#ifndef USBLISTENER_H
#define USBLISTENER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "../settings.hpp"
#include "../ui/DialogDevicePick/devicepickdialog.hpp"
#include <QtConcurrent/QtConcurrent>
#include "listener.hpp"

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

    QString selectedUSBDevice = "";

signals:
    void updateDeviceList(QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> deviceList);
    void usbSetup(QString deviceID);

private:
    QFuture<void> pollFuture;
    QTimer* pollTimer = new QTimer(this);

    bool usbInitialized = false;
    bool usbSetupActive = false;

    void initADB();

    QStringList getDevices();
    QPair<QList<QPair<QString, Utils::ADB_DEVICE_STATUS>>, QStringList> parseDeviceList(QStringList list);

private slots:
    void start() override;
    void stop() override;

    void usbCheck();
    void usbPrepareDevice(QString deviceID);
};

#endif // USBLISTENER_H
