#include "deviceauthdialog.hpp"
#include "ui_deviceauthdialog.h"

#include <QRandomGenerator>
#include "../../net/exitcode.hpp"
#include "../../openmic.hpp"

DeviceAuthDialog::DeviceAuthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceAuthDialog)
{
    server = &Server::getInstance();
    ui->setupUi(this);

    connect(this, SIGNAL(rejected()), SLOT(onReject()));
    connect(server->handler, SIGNAL(onAuthCodeReceived(int)), this, SLOT(onAuthCodeReceived(int)));

    OpenMic* omic = &OpenMic::getInstance();
    connect(omic, &OpenMic::disconnected, this, [=](){ reject(); });
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

    QJsonObject response;

    if (authCode == generatedCode) {
        qDebug() << "Received authCode is correct!";

        Settings* settings = &Settings::getInstance();

        QString knownIDsRaw = settings->Get(DEVICE_PAIRED).toString();
        QStringList knownIDs = knownIDsRaw.split(PAIRED_DEVICES_SEPERATOR);

        knownIDs.append(server->connectedClientID);
        knownIDsRaw = knownIDs.join(PAIRED_DEVICES_SEPERATOR);

        settings->Set(DEVICE_PAIRED, knownIDsRaw);

        qDebug() << "Added" << server->connectedClientID << "to known devices list!";
        accept();
    } else {
        qDebug() << "Received authCode is invalid!";

        response["error"] = AUTH_CODE_INVALID;
        response["message"] = tr("Code you provided is invalid for this server.");
    }

    server->sendMessage(Handler::GetResponse(AUTH_CODE_VERIFY, response));
}

void DeviceAuthDialog::onReject()
{
    qDebug() << "Auth dialog has been rejected";

    Server* server = &Server::getInstance();
    server->serverDisconnect(CANCELED_AUTH_CODE_DIALOG);
}
