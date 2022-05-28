#ifndef PACKET_H
#define PACKET_H

#include <QObject>

class Packet : public QObject
{
    Q_OBJECT
public:
    explicit Packet(QObject *parent = nullptr);

signals:

};

#endif // PACKET_H
