#ifndef DEVICEAUTHDIALOG_H
#define DEVICEAUTHDIALOG_H

#include <QDialog>

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

    void generateCode();
    int generatedCode;
};

#endif // DEVICEAUTHDIALOG_H
