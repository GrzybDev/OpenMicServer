#ifndef OPENMIC_H
#define OPENMIC_H

#include <QObject>
#include <QWebSocketServer>
#include "settings.h"
#include "net/server.h"

class OpenMic : public QObject
{
    Q_OBJECT
public:
    explicit OpenMic(QObject *parent = nullptr);
    OpenMic(const OpenMic&) {}

    static OpenMic & getInstance() {
        static OpenMic * _instance = nullptr;

        if (_instance == nullptr)
            _instance = new OpenMic();

        return *_instance;
    }

    void RestartServer();

signals:
    void changeConnectionStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText);
    void initError(QString errorText);

private:
    Settings* appSettings;
    Server* server;

    QTimer* btTimer;

    QMap<Server::CONNECTOR, QWebSocketServer*> webSockets;

    void StartServer(bool isPublic);
    void StopServers();

    void initUSB();
};

#endif // OPENMIC_H
