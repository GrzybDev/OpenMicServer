#include "settingsaudio.h"
#include "ui_settingsaudio.h"

#include <QMediaDevices>
#include <QAudioDevice>

SettingsAudio::SettingsAudio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsAudio)
{
    appSettings = & Settings::getInstance();

    ui->setupUi(this);

    applyValuesFromConfig();
}

SettingsAudio::~SettingsAudio()
{
    delete ui;
}

void SettingsAudio::applyValuesFromConfig()
{
    ui->device->clear();
    ui->sampleRate->clear();
    ui->sampleFormat->clear();
    ui->channelConfig->clear();

    QAudioDevice audioDev = setAudioDevices();
    setSampleRates(audioDev);
    setSampleFormats(audioDev);
    setChannelConfigs(audioDev);
}

QAudioDevice SettingsAudio::setAudioDevices()
{
    QMediaDevices mDevices;
    auto deviceList = mDevices.audioOutputs();

    foreach (auto device, deviceList)
        ui->device->addItem(device.description(), device.id());

    QAudioDevice selDev = appSettings->GetAudioDevice();

    int idx = ui->device->findData(selDev.id());
    ui->device->setCurrentIndex(idx);

    return selDev;
}

void SettingsAudio::setSampleRates(QAudioDevice dev)
{
    ui->sampleRate->setMinimum(dev.minimumSampleRate());
    ui->sampleRate->setMaximum(dev.maximumSampleRate());
    ui->sampleRate->setValue(appSettings->Get(AUDIO_SAMPLE_RATE).toInt());
}

void SettingsAudio::setSampleFormats(QAudioDevice dev)
{
    auto formats = dev.supportedSampleFormats();

    foreach (auto format, formats)
    {
        switch (format)
        {
            case QAudioFormat::UInt8:
                ui->sampleFormat->addItem("PCM (8-bit unsigned integers)", format);
                break;
            case QAudioFormat::Int16:
                ui->sampleFormat->addItem("PCM (16-bit signed integers)", format);
                break;
            case QAudioFormat::Int32:
                ui->sampleFormat->addItem("PCM (32-bit signed integers)", format);
                break;
            case QAudioFormat::Float:
                ui->sampleFormat->addItem("PCM (Floats)", format);
                break;
            default:
                break;
        }
    }

    int selFormat = appSettings->Get(AUDIO_SAMPLE_FORMAT).toInt();
    int idx = ui->sampleFormat->findData(selFormat);
    ui->device->setCurrentIndex(idx);
}

void SettingsAudio::setChannelConfigs(QAudioDevice dev)
{
    ui->channelConfig->setMinimum(dev.minimumChannelCount());
    ui->channelConfig->setMaximum(dev.maximumChannelCount());
    ui->channelConfig->setValue(appSettings->Get(AUDIO_CHANNEL_CONFIG).toInt());
}

void SettingsAudio::on_buttonBox_clicked(QAbstractButton *button)
{
    if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
        appSettings->Reset(AUDIO_DEVICE);
        appSettings->Reset(AUDIO_SAMPLE_RATE);
        appSettings->Reset(AUDIO_SAMPLE_FORMAT);
        appSettings->Reset(AUDIO_CHANNEL_CONFIG);

        applyValuesFromConfig();
    } else if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Reset)) {
        applyValuesFromConfig();
    } else if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Discard)) {
        reject();
    } else if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
        QByteArray id = ui->device->currentData().toByteArray();

        QMediaDevices mDevices;
        auto deviceList = mDevices.audioOutputs();

        foreach (auto device, deviceList)
        {
            if (id == device.id())
                appSettings->Set(AUDIO_DEVICE, device.description());
        }

        appSettings->Set(AUDIO_SAMPLE_RATE, ui->sampleRate->value());
        appSettings->Set(AUDIO_SAMPLE_FORMAT, ui->sampleFormat->currentData());
        appSettings->Set(AUDIO_CHANNEL_CONFIG, ui->channelConfig->value());

        accept();
    }
}

