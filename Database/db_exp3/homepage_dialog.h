#ifndef HOMEPAGE_DIALOG_H
#define HOMEPAGE_DIALOG_H

#include <QDialog>

namespace Ui {
class homepage_dialog;
}

class homepage_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit homepage_dialog(QWidget *parent = 0);
    ~homepage_dialog();

private slots:
    void show_itself();
    
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

signals:
    void add_education_experience();
    void add_job_experience();

private:
    Ui::homepage_dialog *ui;
};

#endif // HOMEPAGE_DIALOG_H
