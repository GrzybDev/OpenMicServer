#include "audio.hpp"


Audio::Audio(QObject *parent)
    : QObject{parent}
{
    settings = &Settings::getInstance();
}

void Audio::initialize(int sampleRate, QAudioFormat::ChannelConfig channels, QAudioFormat::SampleFormat format)
{
    audioDev = settings->GetAudioDevice();
    audioFormat = QAudioFormat();

    audioFormat.setSampleRate(sampleRate);
    audioFormat.setChannelConfig(channels);
    audioFormat.setSampleFormat(format);

    audioOut = new QAudioSink(audioDev, audioFormat);
    audioIO = audioOut->start();
}

void Audio::play(QByteArray audioData)
{
    audioIO->write(audioData);
}
