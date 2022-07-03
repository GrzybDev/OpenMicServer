#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include "../../openmic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void changeStatus(Server::CONNECTOR connector, bool isEnabled, QString statusText);
    void showError(QString errorTitle, QString errorText);

private slots:
    void on_actionAudio_triggered();
    void on_actionNetwork_triggered();
    void on_actionDevices_triggered();
    void on_actionSystem_triggered();

    void toggleVisibility();
    void appExit();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
    OpenMic* openmic = &OpenMic::getInstance();
    QSystemTrayIcon* trayIco = new QSystemTrayIcon(QIcon(":/appIcon.ico"), this);

    void initStatus();
    void initTrayIcon();
};
#endif // MAINWINDOW_H
