#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QMovie>
#include <QMessageBox>
#include <QPainter>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>

#include "../SettingsAudio/settingsaudio.hpp"
#include "../SettingsDevices/settingsdevices.hpp"
#include "../SettingsNetwork/settingsnetwork.hpp"
#include "../SettingsSystem/settingssystem.hpp"
#include "../WindowAbout/aboutwindow.hpp"
#include "../../net/server.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initStatus();
    initTrayIcon();

    openmic->RestartServer();

    connect(openmic, &OpenMic::connected, this, &MainWindow::onConnected);
    connect(openmic, &OpenMic::disconnected, this, &MainWindow::onDisconnect);
    connect(openmic, &OpenMic::usbDeviceUpdate, this, &MainWindow::usbDeviceUpdate);

#ifdef Q_OS_WIN
    QAction *vbcableDonate = new QAction(tr("...to VB-Cable Authors"));
    connect(vbcableDonate, SIGNAL(triggered()), this, SLOT(on_action_to_VBCable_authors_triggered()));
    ui->menuDonate->addAction(vbcableDonate);
#endif

    if (openmic->appSettings->Get(SYSTEM_START_MINIMIZED).toBool())
    {
        trayIco->showMessage(tr("OpenMic is ready!"),
                             tr("Server is now waiting for connections!"),
                             QSystemTrayIcon::Information,
                             10000);
    }
    else
    {
        show();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStatus()
{
    QMovie *spinner = new QMovie(":/Spinner-1s-48px.gif");
    spinner->start();

    QList<QLabel*> lblList;
    lblList.append(ui->btStatus);
    lblList.append(ui->usbStatus);
    lblList.append(ui->wifiStatus);

    foreach (QLabel* lbl, lblList)
    {
        lbl->setAttribute(Qt::WA_NoSystemBackground);
        lbl->setCursor(Qt::WhatsThisCursor);
        lbl->setMovie(spinner);
        lbl->setToolTip(tr("Initializing, please wait..."));
    }

    connect(openmic, &OpenMic::changeConnectionStatus, this, &MainWindow::changeStatus);
    connect(openmic, &OpenMic::showError, this, [=](QString errorTitle, QString errorText){ this->showError(errorTitle, errorText); });
}

void MainWindow::initTrayIcon()
{
    QAction *hideAction = new QAction(tr("Show/Hide Control Panel"), trayIco);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(toggleVisibility()));

    QAction *exitAction = new QAction(tr("Exit"), trayIco);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(appExit()));

    QMenu *trayIcoMenu = new QMenu;
    trayIcoMenu->addAction(hideAction);
    trayIcoMenu->addAction(exitAction);
    trayIco->setContextMenu(trayIcoMenu);
    trayIco->show();
}

void MainWindow::toggleVisibility()
{
    if (isVisible())
        hide();
    else
        show();
}

void MainWindow::appExit()
{
    Server* srv = &Server::getInstance();

    if (srv->connectedClient)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Exit from OpenMic"));
        msgBox.setText(tr("You are currently connected to client, if you close OpenMic client will lose connection to server.\nDo you really want to exit OpenMic?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setEscapeButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        if (msgBox.exec() == QMessageBox::No)
            return;
    }

    trayIco->hide();
    close();
}

void MainWindow::showError(QString errorTitle, QString errorText)
{
    QMessageBox::critical(this,
                          errorTitle,
                          errorText);
}

void MainWindow::changeStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText)
{
    QLabel* affectedLbl;

    switch (connector) {
        case Server::BLUETOOTH:
            affectedLbl = ui->btStatus;
            break;
        case Server::USB:
            affectedLbl = ui->usbStatus;
            break;
        case Server::WIFI:
            affectedLbl = ui->wifiStatus;
            break;
        default:
            qCritical() << "Connector doesn't have QLabel to update! (Connector:" << connector << ")";
            return;
    }

    QString pixPath = ":/icons/";

    QPixmap* pix;
    QColor color;

    if (isEnabled) {
        pix = new QPixmap(pixPath + "check_FILL0_wght400_GRAD0_opsz48.svg");
        color = QColor("green");
    } else {
        pix = new QPixmap(pixPath + "block_FILL0_wght400_GRAD0_opsz48.svg");
        color = QColor("red");
    }

    QPainter qp = QPainter(pix);
    qp.setCompositionMode(QPainter::CompositionMode_SourceIn);
    qp.fillRect(pix->rect(), color);
    qp.end();

    affectedLbl->setPixmap(*pix);
    affectedLbl->setToolTip(statusText);
}

void MainWindow::on_actionAudio_triggered()
{
    SettingsAudio settingsAudio;
    settingsAudio.exec();

    openmic->RestartServer();
}


void MainWindow::on_actionNetwork_triggered()
{
    SettingsNetwork settingsNetwork;
    settingsNetwork.exec();

    openmic->RestartServer();
}


void MainWindow::on_actionDevices_triggered()
{
    SettingsDevices settingsDevices;
    settingsDevices.exec();

    openmic->RestartServer();
}


void MainWindow::on_actionSystem_triggered()
{
    SettingsSystem settingsSystem;
    settingsSystem.exec();

    openmic->RestartServer();
}


void MainWindow::on_actionExit_triggered()
{    
    appExit();
}

void MainWindow::onConnected()
{
    ui->actionDisconnect->setEnabled(true);

    if (openmic->appSettings->Get(SYSTEM_MINIMIZE_ON_CONNECT).toBool())
    {
        hide();
    }

    trayIco->showMessage(tr("Client connected!"),
                         tr("Application now will run in background"),
                         QSystemTrayIcon::Information,
                         10000);
}

void MainWindow::onDisconnect()
{
    ui->actionDisconnect->setEnabled(false);

    trayIco->showMessage(tr("Client disconnected!"),
                         tr("Server still works in background, another client can now connect. Click on this icon to show control panel"),
                         QSystemTrayIcon::Information,
                         10000);
}

void MainWindow::on_actionDisconnect_triggered()
{
    openmic->RestartServer();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutWindow win;
    win.exec();
}

void MainWindow::on_action_to_OpenMic_author_triggered()
{
    QDesktopServices::openUrl(QUrl("https://ko-fi.com/grzybdev"));
}

void MainWindow::on_action_to_VBCable_authors_triggered()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&locale.x=us_XC&lc=US&hosted_button_id=AFR983KFQKK2J"));
}

void MainWindow::on_actionSelect_USB_Device_triggered()
{
    DevicePickDialog* devicePickDialog = &DevicePickDialog::getInstance();
    devicePickDialog->showDialog();
}

void MainWindow::usbDeviceUpdate(bool isMultiple)
{
    ui->actionSelect_USB_Device->setEnabled(isMultiple);
}
