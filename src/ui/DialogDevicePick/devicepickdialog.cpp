#include "devicepickdialog.hpp"
#include "ui_devicepickdialog.h"

#include <QPushButton>

DevicePickDialog::DevicePickDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevicePickDialog)
{
    ui->setupUi(this);

    setModal(true);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    appSettings = & Settings::getInstance();
}

DevicePickDialog::~DevicePickDialog()
{
    delete ui;
}

void DevicePickDialog::updateDeviceList(QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> deviceList)
{
    for (int i = ui->deviceList->rowCount() - 1; i >= 0; i--)
        ui->deviceList->removeRow(i);

    foreach (auto device, deviceList) {
        QLabel* deviceID = new QLabel(this);
        deviceID->setAlignment(Qt::AlignCenter);

        QString deviceStr = "<b>" + device.first + "</b><br/>";

        switch (device.second) {
            case Utils::ONLINE:
                deviceStr += tr("Device is ready to use");
                break;
            case Utils::UNAUTHORIZED:
                deviceStr += tr("Device is not authorized");
                break;
            case Utils::OFFLINE:
                deviceStr += tr("Device is in invalid state.");
                break;
        }

        deviceID->setText(deviceStr);

        QPushButton* selectBtn = new QPushButton(this);
        selectBtn->setText(tr("Select this device"));

        if (device.second != Utils::ONLINE)
            selectBtn->setEnabled(false);

        connect(selectBtn, &QPushButton::clicked, this, [=]() { selectDevice(device.first); });

        ui->deviceList->addRow(deviceID, selectBtn);
    }
}

void DevicePickDialog::selectDevice(QString deviceID)
{
    appSettings->Set(USB_AUTO_CONNECT, deviceID);

    accept();
}
