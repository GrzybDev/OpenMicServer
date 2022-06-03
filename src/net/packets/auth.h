#ifndef PACKETAUTH_H
#define PACKETAUTH_H

#include <QObject>
#include "../packet.h"

class PacketAuth : public QObject, public Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)
public:
    explicit PacketAuth(QObject *parent = nullptr);

    QString Handle(MESSAGE type, QJsonObject data);

private:
    QString handleCodeVerify(QJsonObject data);
};

#endif // PACKETAUTH_H
