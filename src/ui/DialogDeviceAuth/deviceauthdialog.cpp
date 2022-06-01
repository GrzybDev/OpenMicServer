#include "deviceauthdialog.h"
#include "ui_deviceauthdialog.h"

#include <QRandomGenerator>

DeviceAuthDialog::DeviceAuthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceAuthDialog)
{
    ui->setupUi(this);

    generateCode();
}

DeviceAuthDialog::~DeviceAuthDialog()
{
    delete ui;
}

void DeviceAuthDialog::generateCode()
{
    QRandomGenerator generator = QRandomGenerator::securelySeeded();
    generatedCode = generator.bounded(PAIR_NUM_MIN, PAIR_NUM_MAX);

    ui->codeLabel->setText(QString::number(generatedCode));
}
