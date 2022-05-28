#ifndef PACKETCLIENTSYSTEM_H
#define PACKETCLIENTSYSTEM_H

#include <QObject>

class PacketClientSystem : public QObject
{
    Q_OBJECT
public:
    explicit PacketClientSystem(QObject *parent = nullptr);

signals:

};

#endif // PACKETCLIENTSYSTEM_H
