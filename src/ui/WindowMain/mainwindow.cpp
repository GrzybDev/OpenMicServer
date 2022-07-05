#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QMovie>
#include <QMessageBox>
#include <QPainter>
#include <QAction>

#include "../SettingsAudio/settingsaudio.hpp"
#include "../SettingsDevices/settingsdevices.hpp"
#include "../SettingsNetwork/settingsnetwork.hpp"
#include "../SettingsSystem/settingssystem.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initStatus();
    initTrayIcon();

    openmic->RestartServer();
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
    QAction *hideAction = new QAction("Show/Hide", trayIco);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(toggleVisibility()));

    QAction *exitAction = new QAction("Exit", trayIco);
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

