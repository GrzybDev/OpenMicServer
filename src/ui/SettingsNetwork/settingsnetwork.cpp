#include "settingsnetwork.h"
#include "ui_settingsnetwork.h"

SettingsNetwork::SettingsNetwork(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsNetwork)
{
    ui->setupUi(this);
}

SettingsNetwork::~SettingsNetwork()
{
    delete ui;
}
