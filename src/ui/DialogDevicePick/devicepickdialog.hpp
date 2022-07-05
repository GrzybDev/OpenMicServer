#ifndef DEVICEPICKDIALOG_H
#define DEVICEPICKDIALOG_H

#include <QDialog>
#include "../../utils.hpp"
#include "../../settings.hpp"

namespace Ui {
class DevicePickDialog;
}

class DevicePickDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DevicePickDialog(QWidget *parent = nullptr);
    ~DevicePickDialog();

public slots:
    void updateDeviceList(QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> deviceList);

private:
    Ui::DevicePickDialog *ui;
    Settings* appSettings;

    void selectDevice(QString deviceID);
};

#endif // DEVICEPICKDIALOG_H
