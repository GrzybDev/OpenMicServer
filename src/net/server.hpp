#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QTimer>
#include "handler.hpp"
#include "errorcode.hpp"
#include "../audio.hpp"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    Server(const Server&): QObject(nullptr) {}

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
    QObject* connectedClient = nullptr;

    void sendMessage(QString message);

    void serverDisconnect(ERROR_CODE errCode, bool waitForMessageSend = false);
    void clientDisconnect();

    QString connectedClientID;
    bool isBroadcast = false;
    bool isAuthorized = false;

    void changeConnectionStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText);
    void showError(QString errorTitle, QString errorText);

signals:
    void onMessageSent();

public slots:
    void onNewConnection(Server::CONNECTOR connector);

private:
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
