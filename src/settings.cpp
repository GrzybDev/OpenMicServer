#include "settings.h"

#include <QIntValidator>
#include <QMessageBox>

Settings::Settings(QObject *parent)
    : QObject{parent}
{
    validateCommunicationPort();
}

QVariant Settings::Get(QString key)
{
    return ctx.value(key, GetDefault(key));
}

QVariant Settings::GetDefault(QString key)
{
    switch (qt_hash(key))
    {
        case qConstHash(NETWORK_PORT):
            return 10000;
        case qConstHash(NETWORK_INTERFACE):
            return getDefaultNetworkAdapter();
        default:
            return QVariant();
    }
}

QNetworkInterface Settings::GetNetworkInterface()
{
    QString ifaceName = Get(NETWORK_INTERFACE).toString();
    return QNetworkInterface::interfaceFromName(ifaceName);
}

void Settings::Reset(QString key)
{
    ctx.remove(key);
}

void Settings::validateCommunicationPort()
{
    // Make sure that config is valid
    QIntValidator portValidator(PORT_MIN, PORT_MAX, this);

    QString communicationPort = Get(NETWORK_PORT).toString();
    int pos = 0;

    if (portValidator.validate(communicationPort, pos) != QValidator::Acceptable)
    {
        // Reset communication port to default settings
        Reset(NETWORK_PORT);

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(tr("OpenMic Server - Configuration validation failed"));
        msgBox.setText(tr("Invalid network communication port specified!"));
        msgBox.setInformativeText(tr("Offline configuration change for communication port in network section were out of acceptable range. Network communication port have been reset to default value."));
        msgBox.setDetailedText(tr("Network communication port have been set to: %1\n\nPrevious (invalid) value: %2 (out of range, valid range: %3-%4)").arg(Get(NETWORK_PORT).toUInt())
                                                                                                                                                       .arg(communicationPort)
                                                                                                                                                       .arg(PORT_MIN)
                                                                                                                                                       .arg(PORT_MAX));

        msgBox.exec();
    }
}

QString Settings::getDefaultNetworkAdapter()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    // Return "first" valid network interface (valid = Up and not Loopback)
    foreach (QNetworkInterface interface, ifaces)
    {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) && !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
            return interface.name();
    }

    // Edge-case: What if there's no network interfaces?
    // (Assuming that every operating system must have at least loopback interface working
    // if we reach here, first network interface will be returned (most probably loopback))

    return ifaces.first().name();
}
