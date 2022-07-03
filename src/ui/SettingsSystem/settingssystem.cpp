#include "settingssystem.h"
#include "ui_settingssystem.h"

SettingsSystem::SettingsSystem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsSystem)
{
    appSettings = & Settings::getInstance();

    ui->setupUi(this);

    applyValuesFromConfig();
}

SettingsSystem::~SettingsSystem()
{
    delete ui;
}

void SettingsSystem::applyValuesFromConfig()
{
    ui->launchOS->setChecked(appSettings->willAppRunOnSystemStartup());
    ui->launchMinimize->setChecked(appSettings->Get(SYSTEM_START_MINIMIZED).toBool());
    ui->minimizeConnect->setChecked(appSettings->Get(SYSTEM_MINIMIZE_ON_CONNECT).toBool());
}

void SettingsSystem::on_buttonBox_clicked(QAbstractButton *button)
{
    if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        appSettings->setAutostart(ui->launchOS->isChecked());
        appSettings->Set(SYSTEM_START_MINIMIZED, ui->launchMinimize->isChecked());
        appSettings->Set(SYSTEM_MINIMIZE_ON_CONNECT, ui->minimizeConnect->isChecked());

        accept();
    }
}

