#include "usblistener.h"

#include "../openmic.h"

USBListener::USBListener(QObject *parent)
    : QObject{parent}
{
    appSettings = & Settings::getInstance();

    OpenMic* omic = &OpenMic::getInstance();
    connect(this, &USBListener::updateDeviceList, omic->devicePickDialog, &DevicePickDialog::updateDeviceList);
    connect(this, SIGNAL(stopListener()), SLOT(stop()));
}

USBListener::~USBListener()
{
    stop();
}

void USBListener::stop()
{
    pollTimer->stop();
    pollFuture.cancel();
}

void USBListener::start()
{
    pollTimer = new QTimer();
    connect(pollTimer, &QTimer::timeout, this, &USBListener::usbCheck);
    pollTimer->start(USB_CHECK_INTERVAL);
}

void USBListener::initADB()
{
    QProcess* adb = new QProcess();

    QString program = "adb";
    QStringList arguments;
    arguments << "start-server";

    adb->start(program, arguments);

    OpenMic* omic = &OpenMic::getInstance();

    if (!adb->waitForStarted())
    {
        QString errorStr = tr("Failed to start ADB!\nEither adb binary is missing, or you may have insufficient permissions to invoke the program.");

        emit omic->changeConnectionStatus(Server::USB, false, errorStr);
        emit omic->showError(tr("USB Initialization failed"), errorStr + tr("\n\nYou won't be able to connect via USB! (If you don't intend to use USB, you can disable it in Settings -> Device)"));

        emit stopListener();
        return;
    }

    if (!adb->waitForFinished())
    {
        QString errorStr = tr("ADB took too long to execute, cannot continue USB initialization.");

        emit omic->changeConnectionStatus(Server::USB, false, errorStr);
        emit omic->showError(tr("USB Initialization failed"), errorStr + tr("\n\nYou won't be able to connect via USB! (If you don't intend to use USB, you can disable it in Settings -> Device)"));

        emit stopListener();
        return;
    }

    omic->StartWebSocketServer(QHostAddress::LocalHost, Server::USB);
    usbInitialized = true;
}

void USBListener::usbCheck()
{
    if (pollFuture.isFinished()) {
        pollFuture = QtConcurrent::run([=]() {
            if (!usbInitialized) {
                initADB();
                return;
            }

            QProcess* adb = new QProcess();

            QString program = "adb";
            QStringList arguments;
            arguments << "devices";

            adb->start(program, arguments);

            OpenMic* omic = &OpenMic::getInstance();

            if (!adb->waitForStarted()) {
                QString errorStr = tr("Failed to get device list from ADB because executable became unavailable, disabled USB functionality");

                emit omic->changeConnectionStatus(Server::USB, false, errorStr);
                emit omic->showError(tr("Fetch device failed"), errorStr + tr("\n\nYou won't be able to connect via USB! (If you don't intend to use USB, you can disable it in Settings -> Device)"));

                emit stopListener();
                return;
            }

            if (!adb->waitForFinished()) {
                QString errorStr = tr("ADB took too long to return list of devices, disabled USB functionality");

                emit omic->changeConnectionStatus(Server::USB, false, errorStr);
                emit omic->showError(tr("Fetch device failed"), errorStr + tr("\n\nYou won't be able to connect via USB! (If you don't intend to use USB, you can disable it in Settings -> Device)"));

                emit stopListener();
                return;
            }

            QString output = adb->readAllStandardOutput();
            static QRegularExpression regExp = QRegularExpression("\n|\r\n|\r");
            QStringList devicesStr = output.split(regExp, Qt::SkipEmptyParts);
            devicesStr.removeFirst();

            QList<QPair<QString, Utils::ADB_DEVICE_STATUS>> devices;
            QList<QString> deviceIDs;

            foreach(QString device, devicesStr)
            {
                QStringList deviceInfoList = device.split("\t");
                QPair<QString, Utils::ADB_DEVICE_STATUS> deviceInfo;

                deviceInfo.first = deviceInfoList.first();

                QString statusStr = deviceInfoList.at(1);

                if (statusStr == "device") {
                    deviceInfo.second = Utils::ONLINE;
                } else if (statusStr == "unauthorized") {
                    deviceInfo.second = Utils::UNAUTHORIZED;
                } else {
                    deviceInfo.second = Utils::OFFLINE;
                }

                devices.append(deviceInfo);
                deviceIDs.append(deviceInfo.first);
            }

            emit updateDeviceList(devices);

            if (deviceIDs.contains(selectedUSBDevice))
            {
                // We already set up reverse port to this device, do nothing
                return;
            } else {
                selectedUSBDevice = "";
            }

            if (devices.size() > 1) {
                // More than one device connected, either pick previously selected or show select screen
                QString previousDeviceID = appSettings->Get(USB_AUTO_CONNECT).toString();

                if (deviceIDs.contains(previousDeviceID)) {
                    foreach (auto device, devices)
                    {
                        if (device.first == previousDeviceID)
                        {
                            if (device.second != Utils::ONLINE)
                            {
                                qWarning() << "Temporarily skipping" << device.first << "because it's not in valid state!";
                                return;
                            }
                        }
                    }

                    usbPrepare(previousDeviceID);
                } else {
                    omic->devicePickDialog->show();
                }
            } else if (devices.size() == 1) {
                auto device = devices.first();

                if (device.second == Utils::ONLINE) {
                    usbPrepare(device.first);
                } else {
                    qDebug() << "Skipping" << device.first << "because device is in invalid state!";
                }
            }
        });
    }
}

void USBListener::usbPrepare(QString deviceID)
{
    QProcess* adb = new QProcess();

    QString program = "adb";
    QStringList arguments;

    ushort deviceCommunicationPort = appSettings->Get(NETWORK_PORT).toUInt();
    QString reversePort = "tcp:" + QString::number(deviceCommunicationPort);
    arguments << "reverse" << "--remove" << reversePort;

    adb->start(program, arguments);
    adb->waitForFinished();

    arguments.clear();
    arguments << "-s" << deviceID << "reverse" << reversePort << reversePort;

    adb->start(program, arguments);

    if (!adb->waitForFinished() || adb->exitCode() != 0) {
        qCritical() << "Failed to set up port forwarding to USB device! (" << adb->readAllStandardError() << ")";
    } else {
        qDebug() << "Successfully set up port forwarding for device" << deviceID;
        selectedUSBDevice = deviceID;
    }
}
