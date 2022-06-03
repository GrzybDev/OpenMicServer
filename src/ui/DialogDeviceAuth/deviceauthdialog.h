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

    DeviceAuthDialog(const DeviceAuthDialog&) {}

    static DeviceAuthDialog & getInstance() {
        static DeviceAuthDialog * _instance = 0;

        if ( _instance == 0 )
            _instance = new DeviceAuthDialog();

        return *_instance;
    }

    void generateCode();

private:
    Ui::DeviceAuthDialog *ui;
    Server* server;

    int generatedCode;

private slots:
    void onAuthCodeReceived(int authCode);
    void onReject();
};

#endif // DEVICEAUTHDIALOG_H
