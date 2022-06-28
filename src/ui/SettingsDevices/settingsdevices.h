#ifndef SETTINGSDEVICES_H
#define SETTINGSDEVICES_H

#include <QDialog>

namespace Ui {
class SettingsDevices;
}

class SettingsDevices : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDevices(QWidget *parent = nullptr);
    ~SettingsDevices();

private:
    Ui::SettingsDevices *ui;
};

#endif // SETTINGSDEVICES_H
