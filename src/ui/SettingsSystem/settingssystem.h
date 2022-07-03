#ifndef SETTINGSSYSTEM_H
#define SETTINGSSYSTEM_H

#include <QDialog>
#include <QAbstractButton>
#include "../../settings.h"

namespace Ui {
class SettingsSystem;
}

class SettingsSystem : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsSystem(QWidget *parent = nullptr);
    ~SettingsSystem();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SettingsSystem *ui;
    Settings* appSettings;

    void applyValuesFromConfig();
};

#endif // SETTINGSSYSTEM_H
