#ifndef DEVICEAUTHDIALOG_H
#define DEVICEAUTHDIALOG_H

#include <QDialog>

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
};

#endif // DEVICEAUTHDIALOG_H
