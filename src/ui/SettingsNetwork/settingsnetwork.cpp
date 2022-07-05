#include "settingsnetwork.hpp"
#include "ui_settingsnetwork.h"

SettingsNetwork::SettingsNetwork(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsNetwork)
{
    appSettings = & Settings::getInstance();

    ui->setupUi(this);

    applyValuesFromConfig();
}

SettingsNetwork::~SettingsNetwork()
{
    delete ui;
}

void SettingsNetwork::applyValuesFromConfig()
{
   setNetworkDevices();

   ui->communicationPort->setMinimum(PORT_MIN);
   ui->communicationPort->setMaximum(PORT_MAX);
   ui->communicationPort->setValue(appSettings->Get(NETWORK_PORT).toUInt());

   ui->pingInterval->setValue(appSettings->Get(NETWORK_PING_INTERVAL).toDouble() / 1000);

   ui->usbSupport->setChecked(appSettings->Get(SUPPORT_USB).toBool());
   ui->usbPoll->setValue(appSettings->Get(SUPPORT_USB_POLL_INTERVAL).toDouble() / 1000);
   ui->wifiSupport->setChecked(appSettings->Get(SUPPORT_WIFI).toBool());
   ui->wifiBroadcast->setValue(appSettings->Get(SUPPORT_WIFI_BROADCAST_INTERVAL).toDouble() / 1000);
   ui->btSupport->setChecked(appSettings->Get(SUPPORT_BT).toBool());
   ui->btCheck->setValue(appSettings->Get(SUPPORT_BT_CHECK_INTERVAL).toDouble() / 1000);
}

void SettingsNetwork::setNetworkDevices()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    foreach (QNetworkInterface interface, ifaces)
        ui->netIface->addItem(interface.humanReadableName() + " (" + interface.name() + ")", interface.name());

    QNetworkInterface currNetIface = appSettings->GetNetworkInterface();

    int idx = ui->netIface->findData(currNetIface.name());
    ui->netIface->setCurrentIndex(idx);
}

void SettingsNetwork::on_buttonBox_clicked(QAbstractButton *button)
{
    if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
        appSettings->Reset(NETWORK_INTERFACE);
        appSettings->Reset(NETWORK_PORT);
        appSettings->Reset(NETWORK_PING_INTERVAL);
        appSettings->Reset(SUPPORT_USB);
        appSettings->Reset(SUPPORT_USB_POLL_INTERVAL);
        appSettings->Reset(SUPPORT_WIFI);
        appSettings->Reset(SUPPORT_WIFI_BROADCAST_INTERVAL);
        appSettings->Reset(SUPPORT_BT);
        appSettings->Reset(SUPPORT_BT_CHECK_INTERVAL);

        applyValuesFromConfig();
    } else if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Reset)) {
        applyValuesFromConfig();
    } else if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Discard)) {
        reject();
    } else if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
        appSettings->Set(NETWORK_INTERFACE, ui->netIface->currentData());
        appSettings->Set(NETWORK_PORT, ui->communicationPort->text());
        appSettings->Set(NETWORK_PING_INTERVAL, ui->pingInterval->value() * 1000);
        appSettings->Set(SUPPORT_USB, ui->usbSupport->isChecked());
        appSettings->Set(SUPPORT_USB_POLL_INTERVAL, ui->usbPoll->value() * 1000);
        appSettings->Set(SUPPORT_WIFI, ui->wifiSupport->isChecked());
        appSettings->Set(SUPPORT_WIFI_BROADCAST_INTERVAL, ui->wifiBroadcast->value() * 1000);
        appSettings->Set(SUPPORT_BT, ui->btSupport->isChecked());
        appSettings->Set(SUPPORT_BT_CHECK_INTERVAL, ui->btCheck->value() * 1000);

        accept();
    }
}

