#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include <QDialog>

namespace Ui {
class register_dialog;
}

class register_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit register_dialog(QWidget *parent = 0);
    ~register_dialog();
    
private slots:
    void on_pushButton_clicked();
    void show_itself();

    void on_pushButton_2_clicked();

signals:
    void show_login();

private:
    Ui::register_dialog *ui;
};

#endif // REGISTER_DIALOG_H
