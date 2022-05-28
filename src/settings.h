#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QNetworkInterface>

#define NETWORK_INTERFACE "network/interface"
#define NETWORK_PORT "network/port"

#define PORT_MIN 1024
#define PORT_MAX 65535

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

    QVariant Get(QString key);
    QVariant GetDefault(QString key);
    QNetworkInterface GetNetworkInterface();

    void Reset(QString key);

private:
    QSettings ctx;

    void validateCommunicationPort();
    QString getDefaultNetworkAdapter();
};

uint constexpr qConstHash(const char *string)
{
    uint h = 0;

    while (*string != 0)
    {
        h = (h << 4) + *string++;
        h ^= (h & 0xf0000000) >> 23;
        h &= 0x0fffffff;
    }

    return h;
}

#endif // SETTINGS_H
