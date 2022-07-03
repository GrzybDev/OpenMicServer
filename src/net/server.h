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
        static Server * _instance = nullptr;

        if ( _instance == nullptr )
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

    void changeConnectionStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText);
    void showError(QString errorTitle, QString errorText);

signals:
    void onMessageSent();
    void onDisconnected();

public slots:
    void onNewConnection(Server::CONNECTOR connector);

private:
    QObject* connectedClient = nullptr;
    CONNECTOR connectedVia;
    bool isClientConnected = false;

    QTimer* pingTimer;
    Audio* audioHandler;

private slots:
    void processBluetooth();
    void processCommand(QString message);
    void processAudioData(QByteArray message);
    void socketDisconnected();
    void ping();
};

#endif // SERVER_H
