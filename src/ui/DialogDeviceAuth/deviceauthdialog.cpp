#include "deviceauthdialog.h"
#include "ui_deviceauthdialog.h"

#include <QRandomGenerator>

DeviceAuthDialog::DeviceAuthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceAuthDialog)
{
    server = &Server::getInstance();
    ui->setupUi(this);

    connect(server->handler, SIGNAL(onAuthCodeReceived(int)), this, SLOT(onAuthCodeReceived(int)));

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

void DeviceAuthDialog::onAuthCodeReceived(int authCode)
{
    qDebug() << "Received authCode:" << authCode;

    if (authCode == generatedCode) {
        qDebug() << "Received authCode is correct!";
    }
}
