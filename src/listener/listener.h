#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QHostAddress>
#include "../net/server.h"

class Listener : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    QWebSocketServer* webSocket;
    Settings* appSettings = & Settings::getInstance();

signals:
    void StartListener();
    void StopListener();

protected:
    bool startWebSocket(QHostAddress hostAddress, Server::CONNECTOR connector)
    {
        Server* server = & Server::getInstance();

        webSocket = new QWebSocketServer(SERVER_ID, QWebSocketServer::NonSecureMode);
        quint16 port = appSettings->Get(NETWORK_PORT).toUInt();

        bool listening = webSocket->listen(hostAddress, port);

        if (listening)
        {
            qDebug() << "Server is now listening on" << hostAddress << ":" << port;

            connect(webSocket, &QWebSocketServer::newConnection, server, [=]() { server->onNewConnection(connector); });
            connect(webSocket, &QWebSocketServer::closed, server, &Server::onClosed);

            server->changeConnectionStatus(connector, true, tr("Waiting for your mobile device at %1:%2").arg(hostAddress.toString()).arg(port));
        }
        else
        {
            QString errorString = webSocket->errorString();
            qDebug() << "Failed to start server on" << hostAddress << ":" << port << "(" << errorString << ")";

            server->changeConnectionStatus(connector, false, errorString);
            server->showError(tr("Listener error"), tr("Failed to start listener (ID: %1)!\nPlease check if no other program is currently listening on your current communication port!\n\nSystem returned this error: %2").arg(QString::number(connector), errorString));
        }

        return listening;
    }

private slots:
    virtual void start() = 0;
    virtual void stop() = 0;
};

Q_DECLARE_INTERFACE(Listener, "pl.grzybdev.openmic.server.listener")

#endif // LISTENER_H
