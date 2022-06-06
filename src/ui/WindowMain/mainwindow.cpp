#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initStatus();

    openmic = new OpenMic(this);
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
        lbl->setMovie(spinner);
    }
}
