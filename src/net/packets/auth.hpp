#ifndef PACKETAUTH_H
#define PACKETAUTH_H

#include <QObject>
#include "../packet.hpp"

class PacketAuth : public Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)
public:
    explicit PacketAuth(QObject *parent = nullptr);
    PacketAuth(const PacketAuth&): PacketAuth(nullptr) {}

    static PacketAuth & getInstance() {
        static PacketAuth * _instance = 0;

        if ( _instance == 0 )
            _instance = new PacketAuth();

        return *_instance;
    }

    QString Handle(MESSAGE type, QJsonObject data) override;

    QString handleClientSide(QJsonObject data);

private:
    QString handleCodeVerify(QJsonObject data);
};

#endif // PACKETAUTH_H
