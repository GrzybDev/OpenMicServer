#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QNetworkInterface>
#include <QAudioDevice>
#include <QMediaDevices>

#define AUDIO_DEVICE "audio/device"
#define AUDIO_SAMPLE_RATE "audio/sampleRate"
#define AUDIO_SAMPLE_FORMAT "audio/sampleFormat"
#define AUDIO_CHANNEL_CONFIG "audio/channelConfig"
#define DEVICE_REQUIRE_PAIRING "devices/requirePairing"
#define DEVICE_PAIRED "devices/paired"
#define DEVICE_ID "devices/serverID"
#define NETWORK_INTERFACE "network/interface"
#define NETWORK_PORT "network/port"
#define NETWORK_PING_INTERVAL "network/pingInterval"
#define SUPPORT_USB "support/usb"
#define SUPPORT_USB_POLL_INTERVAL "support/usbInterval"
#define SUPPORT_WIFI "support/wifi"
#define SUPPORT_WIFI_BROADCAST_INTERVAL "support/wifiInterval"
#define SUPPORT_BT "support/bluetooth"
#define SUPPORT_BT_CHECK_INTERVAL "support/bluetoothInterval"
#define USB_AUTO_CONNECT "autoConnectUSB"
#define SYSTEM_START_MINIMIZED "startMinimized"
#define SYSTEM_MINIMIZE_ON_CONNECT "minimizeOnConnect"

#define PAIRED_DEVICES_SEPERATOR ";"
#define BROADCAST_DATA_SEPERATOR ";"

#define PORT_MIN 1024
#define PORT_MAX 65535

#define SERVER_ID "pl.grzybdev.openmic.server"
#define SERVER_GUID "1bc0f9db-4faf-421d-8b21-455c03d890e1"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);
    Settings(const Settings&): QObject(nullptr) {}

    static Settings & getInstance() {
        static Settings * _instance = 0;

        if ( _instance == 0 )
            _instance = new Settings();

        return *_instance;
    }

    QVariant Get(QString key);
    QVariant GetDefault(QString key);

    QNetworkInterface GetNetworkInterface();
    QAudioDevice GetAudioDevice();

    void Set(QString key, QVariant value);
    void Reset(QString key);

    bool willAppRunOnSystemStartup();
    void setAutostart(bool autostart);

private:
    QSettings ctx;
    QMediaDevices* mDevices = new QMediaDevices(this);

    void validateCommunicationPort();
    QString getDefaultNetworkAdapter();
};

#endif // SETTINGS_H
