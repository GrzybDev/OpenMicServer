#ifndef SETTINGSAUDIO_H
#define SETTINGSAUDIO_H

#include <QDialog>
#include "../../settings.h"
#include <QAbstractButton>

namespace Ui {
class SettingsAudio;
}

class SettingsAudio : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsAudio(QWidget *parent = nullptr);
    ~SettingsAudio();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SettingsAudio *ui;
    Settings* appSettings;

    void applyValuesFromConfig();

    QAudioDevice setAudioDevices();
    void setSampleRates(QAudioDevice dev);
    void setSampleFormats(QAudioDevice dev);
    void setChannelConfigs(QAudioDevice dev);
};

#endif // SETTINGSAUDIO_H
