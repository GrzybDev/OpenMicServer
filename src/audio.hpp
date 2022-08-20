#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include "settings.hpp"

#include <QAudioSink>

enum AUDIO_CHANNELS
{
    MONO = 1,
    STEREO = 2
};

enum AUDIO_FORMAT
{
    PCM_8BIT = 0,
    PCM_16BIT = 1,
    PCM_32BIT = 2,
    PCM_FLOAT = 3
};

class Audio : public QObject
{
    Q_OBJECT
public:
    explicit Audio(QObject *parent = nullptr);
    Audio(const Audio&): QObject(nullptr) {}

    static Audio & getInstance() {
        static Audio * _instance = nullptr;

        if (_instance == nullptr)
            _instance = new Audio();

        return *_instance;
    }

    void initialize(int sampleRate, QAudioFormat::ChannelConfig channels, QAudioFormat::SampleFormat format);
    void play(QByteArray audioData);
private:
    Settings* settings;

    QAudioDevice audioDev;
    QAudioFormat audioFormat;
    QAudioSink* audioOut;
    QIODevice* audioIO;
};

#endif // AUDIO_H
