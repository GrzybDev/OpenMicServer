#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDialog>

namespace Ui {
class AboutWindow;
}

class AboutWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow();

private slots:
    void on_authorWebsite_clicked();
    void on_srcCode_clicked();
    void on_linkedin_clicked();
    void on_reddit_clicked();
    void on_twitter_clicked();

private:
    Ui::AboutWindow *ui;
};

#endif // ABOUTWINDOW_H
