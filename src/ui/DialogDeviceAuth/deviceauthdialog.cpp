#include "deviceauthdialog.h"
#include "ui_deviceauthdialog.h"

DeviceAuthDialog::DeviceAuthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceAuthDialog)
{
    ui->setupUi(this);
}

DeviceAuthDialog::~DeviceAuthDialog()
{
    delete ui;
}
