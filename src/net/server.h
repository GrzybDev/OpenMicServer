#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include "handler.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    Server(const Server&) {}

    static Server & getInstance() {
        static Server * _instance = 0;

        if ( _instance == 0 )
            _instance = new Server();

        return *_instance;
    }

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
    void ping();
private:
    QWebSocketServer* context;
    Handler* handler;

    bool isClientConnected = false;
};

#endif // SERVER_H
