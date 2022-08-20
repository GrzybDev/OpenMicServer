#ifndef STREAM_H
#define STREAM_H

#include "../../settings.hpp"
#include "../packet.hpp"

class PacketStream : public Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)
public:
    PacketStream(QObject *parent = nullptr);

    QString Handle(MESSAGE type, QJsonObject data) override;

private:
    Settings *appSettings;

    QString handleStart(QJsonObject data);
};

#endif // STREAM_H
