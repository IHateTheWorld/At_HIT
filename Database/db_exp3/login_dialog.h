#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>

namespace Ui {
class login_dialog;
}

class login_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit login_dialog(QWidget *parent = 0);
    ~login_dialog();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void show_itself();

signals:
    void show_register();
    void show_homepage();

private:
    Ui::login_dialog *ui;
};

#endif // LOGIN_DIALOG_H
