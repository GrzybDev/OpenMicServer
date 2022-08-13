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

    DevicePickDialog(const DevicePickDialog&): QDialog(nullptr) {}

    static DevicePickDialog & getInstance() {
        static DevicePickDialog * _instance = nullptr;

        if ( _instance == nullptr )
            _instance = new DevicePickDialog();

        return *_instance;
    }

    void showDialog();

public slots:
    void updateDeviceList(QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> deviceList);

private:
    Ui::DevicePickDialog *ui;
    Settings* appSettings;
    QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> lastList;

    void selectDevice(QString deviceID);
};

#endif // DEVICEPICKDIALOG_H
