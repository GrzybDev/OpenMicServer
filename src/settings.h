#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QNetworkInterface>

#define NETWORK_INTERFACE "network/interface"
#define NETWORK_PORT "network/port"
#define DEVICE_ID "deviceID"
#define PAIRED_DEVICES "devices/paired"

#define PORT_MIN 1024
#define PORT_MAX 65535

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

    void Set(QString key, QVariant value);
    void Reset(QString key);

private:
    QSettings ctx;

    void validateCommunicationPort();
    QString getDefaultNetworkAdapter();
};

#endif // SETTINGS_H
