#include "audio.hpp"


Audio::Audio(QObject *parent)
    : QObject{parent}
{
    settings = &Settings::getInstance();

    initialize();
}

void Audio::initialize()
{
    audioDev = settings->GetAudioDevice();
    audioFormat = QAudioFormat();

    audioFormat.setSampleRate(8000);
    audioFormat.setSampleFormat(QAudioFormat::Int16);
    audioFormat.setChannelConfig(QAudioFormat::ChannelConfigMono);
    audioFormat.setChannelCount(1);

    audioOut = new QAudioSink(audioDev, audioFormat);
    audioIO = audioOut->start();
}

void Audio::play(QByteArray audioData)
{
    audioIO->write(audioData);
}
