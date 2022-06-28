#ifndef SETTINGSSYSTEM_H
#define SETTINGSSYSTEM_H

#include <QDialog>

namespace Ui {
class SettingsSystem;
}

class SettingsSystem : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsSystem(QWidget *parent = nullptr);
    ~SettingsSystem();

private:
    Ui::SettingsSystem *ui;
};

#endif // SETTINGSSYSTEM_H
