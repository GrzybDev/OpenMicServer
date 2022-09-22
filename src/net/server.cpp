#include "server.hpp"

#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBluetoothSocket>
#include "../listener/bluetoothlistener.hpp"

#include "../openmic.hpp"

Server::Server(QObject *parent)
    : QObject{parent}
{
    handler = new Handler(this);
    audioHandler = &Audio::getInstance();

    pingTimer = new QTimer(this);
    connect(pingTimer, &QTimer::timeout, this, &Server::ping);
}

void Server::onNewConnection(Server::CONNECTOR connector)
{
    OpenMic* openmic = &OpenMic::getInstance();
    QObject* socket;

    if (connector == Server::BLUETOOTH) {
        BluetoothListener* listener = qobject_cast<BluetoothListener*>(openmic->listeners[connector]);
        socket = listener->rfcommServer->nextPendingConnection();
    } else {
        socket = openmic->listeners[connector]->webSocket->nextPendingConnection();
    }

    if (isClientConnected) {
        qDebug() << "One client is already connected (current connector:" << connectedVia << ", requested connector:" << connector << "), cannot handle additional one, disconnecting...";

        if (connector == Server::BLUETOOTH) {
            QBluetoothSocket* btSocket = qobject_cast<QBluetoothSocket*>(socket);
            btSocket->close();
        } else {
            QWebSocket* webSocket = qobject_cast<QWebSocket*>(socket);
            webSocket->close(QWebSocketProtocol::CloseCodePolicyViolated, tr("Connection denied. Another client is already connected!"));
        }

        return;
    }

    isClientConnected = true;
    connectedClient = socket;
    connectedVia = connector;

    if (connector == Server::BLUETOOTH) {
        QBluetoothSocket* btSocket = qobject_cast<QBluetoothSocket*>(socket);
        qDebug() << "Client connected (Connection type:" << connector << ", client address:" << btSocket->peerName() << ")";

        connect(btSocket, &QBluetoothSocket::readyRead, this, &Server::processBluetooth);
        connect(btSocket, &QBluetoothSocket::disconnected, this, &Server::socketDisconnected);
    } else {
        QWebSocket* webSocket = qobject_cast<QWebSocket*>(socket);
        qDebug() << "Client connected (Connection type:" << connector << ", client address:" << webSocket->peerAddress() << ")";

        connect(webSocket, &QWebSocket::textMessageReceived, this, &Server::processCommand);
        connect(webSocket, &QWebSocket::binaryMessageReceived, this, &Server::processAudioData);
        connect(webSocket, &QWebSocket::disconnected, this, &Server::socketDisconnected);
    }

    Settings* settings = &Settings::getInstance();
    pingTimer->start(settings->Get(NETWORK_PING_INTERVAL).toUInt());
}

void Server::processBluetooth()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(connectedClient);

    QByteArray message = socket->readAll();
    QJsonDocument jsonCommand = QJsonDocument::fromJson(message.trimmed());

    if (jsonCommand.isNull())
        processAudioData(message);
    else
        processCommand(QString::fromUtf8(message.constData(), message.length()));
}

void Server::processCommand(QString message)
{
    qDebug() << "<--" << message;

    QJsonDocument jsonCommand = QJsonDocument::fromJson(message.toUtf8());

    if (jsonCommand.isNull()) {
        qDebug() << "Received invalid text data! Disconnecting...";

        if (connectedVia == Server::BLUETOOTH) {
            QBluetoothSocket* btSocket = qobject_cast<QBluetoothSocket *>(connectedClient);
            btSocket->close();
        } else {
            QWebSocket* webSocket = qobject_cast<QWebSocket*>(connectedClient);
            webSocket->close(QWebSocketProtocol::CloseCodeProtocolError, tr("Server received invalid text data and closed connection as a result"));
        }
    } else {
        QJsonObject jsonObject = jsonCommand.object();
        QString response = handler->HandleCommand(jsonObject);

        if (response == "") {
            if (isClientConnected) {
                qDebug() << "No response generated. Disconnecting...";

                if (connectedVia == Server::BLUETOOTH) {
                    QBluetoothSocket* btSocket = qobject_cast<QBluetoothSocket *>(connectedClient);
                    btSocket->close();
                } else {
                    QWebSocket* webSocket = qobject_cast<QWebSocket*>(connectedClient);
                    webSocket->close(QWebSocketProtocol::CloseCodeBadOperation, tr("Server did not recognize command sent by client and closed connection as a result"));
                }
            }
        } else {
            if (response == "DELAYED_RESPONSE") {
                // Other module will send response
                return;
            }

            qDebug() << "-->" << response;

            if (connectedVia == Server::BLUETOOTH) {
                QBluetoothSocket* btSocket = qobject_cast<QBluetoothSocket *>(connectedClient);
                btSocket->write(response.toUtf8());
            } else {
                QWebSocket* webSocket = qobject_cast<QWebSocket*>(connectedClient);
                webSocket->sendTextMessage(response);
            }

            emit onMessageSent();
        }
    }
}

void Server::processAudioData(QByteArray message)
{
    if (isAuthorized)
        audioHandler->play(message);
}

void Server::socketDisconnected()
{
    qDebug() << "Client disconnected!";
    isAuthorized = false;

    if (connectedClient) {
        isClientConnected = false;

        connectedClient->deleteLater();
        connectedClient = nullptr;

        pingTimer->stop();
    }

    if (!isBroadcast) {
        OpenMic* openmic = &OpenMic::getInstance();
        emit openmic->disconnected();
    }
}

void Server::sendMessage(QString message)
{
    if (isClientConnected) {
        qDebug() << "-->" << message;

        if (connectedVia == Server::BLUETOOTH) {
            QBluetoothSocket* btSocket = qobject_cast<QBluetoothSocket *>(connectedClient);
            btSocket->write(message.toUtf8());
        } else {
            QWebSocket* webSocket = qobject_cast<QWebSocket*>(connectedClient);
            webSocket->sendTextMessage(message);
        }
    }
}

void Server::ping()
{
    if (connectedVia == CONNECTOR::BLUETOOTH) {
        pingTimer->stop();
        return;
    }

    QWebSocket* webSocket = qobject_cast<QWebSocket*>(connectedClient);

    if (isClientConnected) {
        webSocket->ping();
    } else {
        pingTimer->stop();
    }
}

void Server::serverDisconnect(ERROR_CODE errCode, bool waitForMessageSend)
{
    if (waitForMessageSend) {
        QMetaObject::Connection * const connection = new QMetaObject::Connection;
        *connection = connect(this, &Server::onMessageSent, this, [=](){
            serverDisconnect(errCode);

            QObject::disconnect(*connection);
            delete connection;
        });

        return;
    }

    if (isClientConnected) {
        QJsonObject data;
        data["exitCode"] = errCode;

        QString packet = Handler::GetResponse(SYSTEM_GOODBYE, data);
        sendMessage(packet);

        clientDisconnect();
    }
}

void Server::clientDisconnect()
{
    if (isClientConnected) {

        if (connectedVia == Server::BLUETOOTH) {
            QBluetoothSocket* btSocket = qobject_cast<QBluetoothSocket *>(connectedClient);
            btSocket->close();
        } else {
            QWebSocket* webSocket = qobject_cast<QWebSocket*>(connectedClient);
            webSocket->close();
        }
    }
}

void Server::changeConnectionStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText)
{
    OpenMic* openmic = &OpenMic::getInstance();
    emit openmic->changeConnectionStatus(connector, isEnabled, statusText);
}

void Server::showError(QString errorTitle, QString errorText)
{
    OpenMic* openmic = &OpenMic::getInstance();
    emit openmic->showError(errorTitle, errorText);
}
