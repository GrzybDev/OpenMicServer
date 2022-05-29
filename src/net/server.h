#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include "handler.h"
#include "../settings.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr, Settings *settings = nullptr);

    enum CONNECTOR
    {
        USB,
        WIFI,
        BLUETOOTH
    };

public slots:
    void onNewConnection(QWebSocketServer* context, Server::CONNECTOR connector);
    void onClosed();

    void processCommand(QString message);
    void processAudioData(QByteArray message);
    void socketDisconnected();

private:
    QWebSocketServer* context;
    Handler* handler;

    bool isClientConnected = false;
};

#endif // SERVER_H
