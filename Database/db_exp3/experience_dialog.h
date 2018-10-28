#ifndef EXPERIENCE_DIALOG_H
#define EXPERIENCE_DIALOG_H

#include <QDialog>

namespace Ui {
class experience_dialog;
}

class experience_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit experience_dialog(QWidget *parent = 0);
    ~experience_dialog();
    
private slots:
    void on_pushButton_clicked();

    void add_eduction_experience();

    void add_job_experience();

    void on_pushButton_2_clicked();

signals:
    void reload_education();

private:
    Ui::experience_dialog *ui;
};

#endif // EXPERIENCE_DIALOG_H
