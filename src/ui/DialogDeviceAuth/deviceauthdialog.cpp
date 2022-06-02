#include "deviceauthdialog.h"
#include "ui_deviceauthdialog.h"

#include <QRandomGenerator>
#include "../../net/exitcode.h"

DeviceAuthDialog::DeviceAuthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceAuthDialog)
{
    server = &Server::getInstance();
    ui->setupUi(this);

    connect(this, SIGNAL(rejected()), SLOT(onReject()));
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

void DeviceAuthDialog::onReject()
{
    qDebug() << "Auth dialog has been rejected";

    Server* server = &Server::getInstance();
    server->serverDisconnect(CANCELED_AUTH_CODE_DIALOG);
}
