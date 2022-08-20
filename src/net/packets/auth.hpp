#ifndef PACKETAUTH_H
#define PACKETAUTH_H

#include <QObject>
#include "../packet.hpp"
#include "../../settings.hpp"

class PacketAuth : public Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)
public:
    explicit PacketAuth(QObject *parent = nullptr);

    QString Handle(MESSAGE type, QJsonObject data) override;

    QString handleAuthClient(QJsonObject data);

private:
    Settings *appSettings;

    QString handleCodeVerify(QJsonObject data);
};

#endif // PACKETAUTH_H
