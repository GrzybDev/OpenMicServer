#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include "../packet.h"

class PacketSystem : public QObject, public Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)

public:
    explicit PacketSystem(QObject *parent = nullptr);

    QString Handle(CLIENT_MESSAGE type, QJsonObject data);

private:
    QString handleHello(QJsonObject data);
};

#endif // SYSTEM_H
