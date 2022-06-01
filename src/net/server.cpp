#include "server.h"

#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

Server::Server(QObject *parent)
    : QObject{parent}
{
    handler = new Handler(this);
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

    qDebug() << "Client connected (Connection type:" << connector << ")";

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
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    qDebug() << "<--" << message;

    QJsonDocument jsonCommand = QJsonDocument::fromJson(message.toUtf8());

    if (jsonCommand.isNull()) {
        qDebug() << "Received invalid text data! Disconnecting...";
        pClient->close(QWebSocketProtocol::CloseCodeProtocolError, tr("Invalid text data, disconnecting..."));
    } else {
        QJsonObject jsonObject = jsonCommand.object();
        QString response = handler->HandleCommand(jsonObject);

        if (response == "") {
            qDebug() << "No response generated. Disconnecting...";
            pClient->close(QWebSocketProtocol::CloseCodeBadOperation, tr("Invalid text data, disconnecting..."));
        } else {
            qDebug() << "-->" << response;
            pClient->sendTextMessage(response);
        }
    }
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
