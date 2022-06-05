#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QTimer>
#include "handler.h"
#include "exitcode.h"
#include "../audio.h"

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

    Handler* handler;

    void sendMessage(QString message);

    void serverDisconnect(EXIT_CODE exitCode, bool waitForMessageSend = false);
    void clientDisconnect();

    QString connectedClientID;

signals:
    void onMessageSent();
    void onDisconnected();

public slots:
    void onNewConnection(QWebSocketServer* context, Server::CONNECTOR connector);
    void onClosed();

private:
    QWebSocket* connectedClient = nullptr;
    QTimer* pingTimer;
    Audio* audioHandler;

    bool isClientConnected = false;

private slots:
    void processCommand(QString message);
    void processAudioData(QByteArray message);
    void socketDisconnected();
    void ping();
};

#endif // SERVER_H
