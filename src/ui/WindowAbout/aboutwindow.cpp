#include "aboutwindow.hpp"
#include "ui_aboutwindow.h"

#include <QDesktopServices>
#include <QUrl>

AboutWindow::AboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::on_authorWebsite_clicked()
{
    QDesktopServices::openUrl(QUrl("https://grzyb.dev"));
}

void AboutWindow::on_openmicWebsite_clicked()
{
    QDesktopServices::openUrl(QUrl("https://openmic.grzyb.dev"));
}

void AboutWindow::on_srcCode_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/GrzybDev/OpenMicServer"));
}

void AboutWindow::on_linkedin_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.linkedin.com/in/grzybdev/"));
}

void AboutWindow::on_reddit_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.reddit.com/user/_GrzybDev_"));
}

void AboutWindow::on_twitter_clicked()
{
    QDesktopServices::openUrl(QUrl("https://twitter.com/GrzybDev"));
}
