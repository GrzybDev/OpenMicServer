#ifndef SETTINGSDEVICES_H
#define SETTINGSDEVICES_H

#include <QDialog>
#include <QAbstractButton>
#include "../../settings.h"

namespace Ui {
class SettingsDevices;
}

class SettingsDevices : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDevices(QWidget *parent = nullptr);
    ~SettingsDevices();

private slots:
    void on_clearAllBtn_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SettingsDevices *ui;
    Settings* appSettings;

    void applyValuesFromConfig();
};

#endif // SETTINGSDEVICES_H
