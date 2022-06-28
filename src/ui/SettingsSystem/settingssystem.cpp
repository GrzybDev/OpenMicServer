#include "settingssystem.h"
#include "ui_settingssystem.h"

SettingsSystem::SettingsSystem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsSystem)
{
    ui->setupUi(this);
}

SettingsSystem::~SettingsSystem()
{
    delete ui;
}
