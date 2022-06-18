#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QNetworkInterface>
#include <QAudioDevice>
#include <QMediaDevices>

#define AUDIO_DEVICE "audio/device"
#define NETWORK_INTERFACE "network/interface"
#define NETWORK_PORT "network/port"
#define NETWORK_PING_INTERVAL "network/pingInterval"
#define DEVICE_ID "deviceID"
#define PAIRED_DEVICES "pairedDevices"

#define PAIRED_DEVICES_SEPERATOR ";"
#define BROADCAST_DATA_SEPERATOR ";"

#define PORT_MIN 1024
#define PORT_MAX 65535

#define WIFI_BROADCAST_INTERVAL 1000
#define BT_CHECK_INTERVAL 1000

#define SERVER_ID "pl.grzybdev.openmic.server"
#define SERVER_GUID "1bc0f9db-4faf-421d-8b21-455c03d890e1"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);
    Settings(const Settings&) {}

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

private:
    QSettings ctx;
    QMediaDevices* mDevices = new QMediaDevices(this);

    void validateCommunicationPort();
    QString getDefaultNetworkAdapter();
};

#endif // SETTINGS_H
