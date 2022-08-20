#include "stream.hpp"

#include "../../audio.hpp"
#include "../errorcode.hpp"
#include "../server.hpp"
#include <QIntValidator>

PacketStream::PacketStream(QObject *parent) : Packet{parent}
{
    appSettings = & Settings::getInstance();
}

QString PacketStream::Handle(MESSAGE type, QJsonObject data)
{
    switch (type) {
        case STREAM_START:
            return handleStart(data);
        default:
            return "";
    }
}

QString PacketStream::handleStart(QJsonObject data)
{
    QString clientPreferredSampleRate = QString::number(data.value("sampleRate").toInt());
    AUDIO_CHANNELS clientPreferredChannels = static_cast<AUDIO_CHANNELS>(data.value("channels").toInt());
    AUDIO_FORMAT clientPreferredFormat = static_cast<AUDIO_FORMAT>(data.value("format").toInt());

    QAudioDevice targetDev = appSettings->GetAudioDevice();

    int serverMinSampleRate = targetDev.minimumSampleRate();
    int serverMaxSampleRate = targetDev.maximumSampleRate();

    int serverMinChannels = targetDev.minimumChannelCount();
    int serverMaxChannels = targetDev.maximumChannelCount();

    auto serverFormats = targetDev.supportedSampleFormats();

    Audio* audioCtx = &Audio::getInstance();

    QJsonObject response;

    QAudioFormat::ChannelConfig channelConfig;
    QAudioFormat::SampleFormat sampleFormat;

    if (appSettings->Get(AUDIO_FORCE_SERVER_CONFIG).toBool())
    {
        int serverSampleRate = appSettings->Get(AUDIO_SAMPLE_RATE).toInt();
        channelConfig = static_cast<QAudioFormat::ChannelConfig>(appSettings->Get(AUDIO_CHANNEL_CONFIG).toInt());
        sampleFormat = static_cast<QAudioFormat::SampleFormat>(appSettings->Get(AUDIO_SAMPLE_FORMAT).toInt());

        audioCtx->initialize(serverSampleRate, channelConfig, sampleFormat);

        response["sampleRate"] = serverSampleRate;
        response["channelConfig"] = channelConfig == QAudioFormat::ChannelConfigMono ? MONO : STEREO;

        if (sampleFormat == QAudioFormat::UInt8)
            response["sampleFormat"] = PCM_8BIT;
        else if (sampleFormat == QAudioFormat::Int16)
            response["sampleFormat"] = PCM_16BIT;
        else if (sampleFormat == QAudioFormat::Int32)
            response["sampleFormat"] = PCM_32BIT;
        else
            response["sampleFormat"] = PCM_FLOAT;
    }
    else
    {
        Server* server = &Server::getInstance();
        int pos = 0;
        QIntValidator v(serverMinSampleRate, serverMaxSampleRate, this);

        if (v.validate(clientPreferredSampleRate, pos) != QValidator::Acceptable)
        {
            response["error"] = NOT_ACCEPTABLE_CONFIG;
            response["message"] = tr("Server does not support client's sample rate, please either change client config or force server audio config");
            server->serverDisconnect(NOT_ACCEPTABLE_CONFIG, true);
            return Handler::GetResponse(STREAM_START, response);
        }

        QString cChannel;

        if (clientPreferredChannels == MONO)
        {
            cChannel = "1";
            channelConfig = QAudioFormat::ChannelConfigMono;
        }
        else
        {
            cChannel = "2";
            channelConfig = QAudioFormat::ChannelConfigStereo;
        }

        pos = 0;
        QIntValidator v2(serverMinChannels, serverMaxChannels, this);

        if (v2.validate(cChannel, pos) != QValidator::Acceptable)
        {
            response["error"] = NOT_ACCEPTABLE_CONFIG;
            response["message"] = tr("Server does not support client's channel config, please either change client config or force server audio config");
            server->serverDisconnect(NOT_ACCEPTABLE_CONFIG, true);
            return Handler::GetResponse(STREAM_START, response);
        }

        if (clientPreferredFormat == PCM_8BIT)
            sampleFormat = QAudioFormat::UInt8;
        else if (clientPreferredFormat == PCM_16BIT)
            sampleFormat = QAudioFormat::Int16;
        else if (clientPreferredFormat == PCM_32BIT)
            sampleFormat = QAudioFormat::Int32;
        else
            sampleFormat = QAudioFormat::Float;

        qDebug() << serverFormats;
        qDebug() << sampleFormat;

        if (!serverFormats.contains(sampleFormat))
        {
            response["error"] = NOT_ACCEPTABLE_CONFIG;
            response["message"] = tr("Server does not support client's audio data format, please either change client config or force server audio config");
            server->serverDisconnect(NOT_ACCEPTABLE_CONFIG, true);
            return Handler::GetResponse(STREAM_START, response);
        }

        audioCtx->initialize(clientPreferredSampleRate.toInt(), channelConfig, sampleFormat);
    }

    return Handler::GetResponse(STREAM_START, response);
}
