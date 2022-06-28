#include "settingsaudio.h"
#include "ui_settingsaudio.h"

SettingsAudio::SettingsAudio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsAudio)
{
    ui->setupUi(this);
}

SettingsAudio::~SettingsAudio()
{
    delete ui;
}
