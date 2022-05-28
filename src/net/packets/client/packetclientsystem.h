#ifndef PACKETCLIENTSYSTEM_H
#define PACKETCLIENTSYSTEM_H

#include <QObject>
#include "../packet.h"

class PacketClientSystem : public QObject, public Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)

public:
    explicit PacketClientSystem(QObject *parent = nullptr);

    QString Handle(CLIENT_MESSAGE type, QJsonObject data);

private:
    QString handleHello(QJsonObject data);
};

#endif // PACKETCLIENTSYSTEM_H
