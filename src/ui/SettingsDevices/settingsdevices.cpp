#include "settingsdevices.h"
#include "ui_settingsdevices.h"

SettingsDevices::SettingsDevices(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDevices)
{
    appSettings = & Settings::getInstance();

    ui->setupUi(this);

    applyValuesFromConfig();
}

SettingsDevices::~SettingsDevices()
{
    delete ui;
}

void SettingsDevices::applyValuesFromConfig()
{
    ui->requirePairing->setChecked(appSettings->Get(DEVICE_REQUIRE_PAIRING).toBool());

    QString devices = appSettings->Get(DEVICE_PAIRED).toString();
    QStringList deviceList = devices.split(PAIRED_DEVICES_SEPERATOR, Qt::SkipEmptyParts);

    QString lblText = tr("Paired devices (Total: %1)").arg(deviceList.size());
    ui->pairedLbl->setText(lblText);
}

void SettingsDevices::on_clearAllBtn_clicked()
{
    appSettings->Reset(DEVICE_PAIRED);

    applyValuesFromConfig();
}

void SettingsDevices::on_buttonBox_clicked(QAbstractButton *button)
{
    if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        appSettings->Set(DEVICE_REQUIRE_PAIRING, ui->requirePairing->isChecked());
    }
}

