#ifndef DEVICEAUTHDIALOG_H
#define DEVICEAUTHDIALOG_H

#include <QDialog>
#include "../../net/server.h"

#define PAIR_NUM_MIN 1000
#define PAIR_NUM_MAX 999999

namespace Ui {
class DeviceAuthDialog;
}

class DeviceAuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceAuthDialog(QWidget *parent = nullptr);
    ~DeviceAuthDialog();

private:
    Ui::DeviceAuthDialog *ui;
    Server* server;

    void generateCode();
    int generatedCode;

private slots:
    void onAuthCodeReceived(int authCode);
};

#endif // DEVICEAUTHDIALOG_H
