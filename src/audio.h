#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include "settings.h"

#include <QAudioSink>

class Audio : public QObject
{
    Q_OBJECT
public:
    explicit Audio(QObject *parent = nullptr);
    Audio(const Audio&) {}

    static Audio & getInstance() {
        static Audio * _instance = nullptr;

        if (_instance == nullptr)
            _instance = new Audio();

        return *_instance;
    }

    void initialize();
    void play(QByteArray audioData);
private:
    Settings* settings;

    QAudioDevice audioDev;
    QAudioFormat audioFormat;
    QAudioSink* audioOut;
    QIODevice* audioIO;
};

#endif // AUDIO_H
