#ifndef SETTINGSNETWORK_H
#define SETTINGSNETWORK_H

#include <QDialog>
#include <QAbstractButton>
#include "../../settings.h"

namespace Ui {
class SettingsNetwork;
}

class SettingsNetwork : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsNetwork(QWidget *parent = nullptr);
    ~SettingsNetwork();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SettingsNetwork *ui;
    Settings* appSettings;

    void applyValuesFromConfig();
};

#endif // SETTINGSNETWORK_H
