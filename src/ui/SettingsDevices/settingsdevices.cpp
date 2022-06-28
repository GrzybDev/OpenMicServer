#include "settingsdevices.h"
#include "ui_settingsdevices.h"

SettingsDevices::SettingsDevices(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDevices)
{
    ui->setupUi(this);
}

SettingsDevices::~SettingsDevices()
{
    delete ui;
}
