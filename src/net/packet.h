#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <QJsonObject>

#include "message.h"

class Packet : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    virtual QString Handle(MESSAGE type, QJsonObject data) = 0;
};

Q_DECLARE_INTERFACE(Packet, "pl.grzybdev.openmic.server.packet")

#endif // PACKET_H
