#include "server.h"

#include <QWebSocket>

Server::Server(QObject *parent)
    : QObject{parent}
{

}

void Server::onNewConnection(QWebSocketServer* context, Server::CONNECTOR connector)
{
    QWebSocket *pSocket = context->nextPendingConnection();

    if (isClientConnected) {
        qDebug() << "One client is already connected, cannot handle additional one, disconnecting...";

        pSocket->close(QWebSocketProtocol::CloseCodePolicyViolated, tr("Connection denied. Another client is already connected!"));
        return;
    }

    isClientConnected = true;

    qDebug() << "Client connected (Connection type:%1" << connector << ")";

    connect(pSocket, &QWebSocket::textMessageReceived, this, &Server::processCommand);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &Server::processAudioData);
    connect(pSocket, &QWebSocket::disconnected, this, &Server::socketDisconnected);
}

void Server::onClosed()
{
    qDebug() << "Connection closed!";
}

void Server::processCommand(QString message)
{
    // QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qDebug() << "Received command:" << message;
}

void Server::processAudioData(QByteArray message)
{
    // QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qDebug() << "Received audio data:" <<  message;
}

void Server::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qDebug() << "Socket disconnected!";

    if (pClient) {
        isClientConnected = false;
        pClient->deleteLater();
    }
}
