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

    void RestartServer();

signals:
private:
    Settings* appSettings;
    Server* server;

    QList<QWebSocketServer*> webSockets;

    void StartServer(bool isPublic);
    void StopServers();
};

#endif // OPENMIC_H