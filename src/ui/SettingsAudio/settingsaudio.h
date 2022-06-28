#ifndef SETTINGSAUDIO_H
#define SETTINGSAUDIO_H

#include <QDialog>

namespace Ui {
class SettingsAudio;
}

class SettingsAudio : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsAudio(QWidget *parent = nullptr);
    ~SettingsAudio();

private:
    Ui::SettingsAudio *ui;
};

#endif // SETTINGSAUDIO_H
