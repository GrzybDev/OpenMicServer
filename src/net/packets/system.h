#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include "../packet.h"
#include "../../settings.h"
#include <QDialog>

class PacketSystem : public QObject, private Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)

public:
    explicit PacketSystem(QObject *parent = nullptr);

    QString Handle(MESSAGE type, QJsonObject data);

private:
    Settings *appSettings;

    QDialog* dialogDeviceAuth;

    QString handleHello(QJsonObject data);
    QString handleGoodbye(QJsonObject data);
};

#endif // SYSTEM_H
