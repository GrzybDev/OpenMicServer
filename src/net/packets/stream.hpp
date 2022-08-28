#ifndef STREAM_H
#define STREAM_H

#include "../../settings.hpp"
#include "../packet.hpp"
#include "../../audio.hpp"

class PacketStream : public Packet
{
    Q_OBJECT
    Q_INTERFACES(Packet)
public:
    PacketStream(QObject *parent = nullptr);

    QString Handle(MESSAGE type, QJsonObject data) override;

private:
    Settings *appSettings;
    Audio *audio;

    QString handleStart(QJsonObject data);
    QString handleVolume(QJsonObject data);
};

#endif // STREAM_H
