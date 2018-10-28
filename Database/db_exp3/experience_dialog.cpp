#include "experience_dialog.h"
#include "ui_experience_dialog.h"
#include "global_info.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QtSql>

experience_dialog::experience_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::experience_dialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("新增个人经历"));
}

experience_dialog::~experience_dialog()
{
    delete ui;
}

void experience_dialog::on_pushButton_clicked()
{
    QString school = ui->lineEdit->text();
    QString start = ui->dateEdit->text();
    QString end = ui->dateEdit_2->text();
    int edu_lev = ui->comboBox->currentIndex();
    QString degree = ui->comboBox_2->currentText();

    qDebug() << "school: " << school;
    qDebug() << "start_date: " << start;
    qDebug() << "end_date: " << end;
    qDebug() << "edu_lev: " << edu_lev;
    qDebug() << "degree: " << degree;

    if(0 == edu_lev)
    {
        QMessageBox::information(this, QObject::tr("教育级别未选"), QObject::tr("请选择教育级别"), QMessageBox::Ok);
        return ;
    }
    if(school == NULL)
    {
        QMessageBox::information(this, QObject::tr("学校为空"), QObject::tr("请输入学校名称"), QMessageBox::Ok);
        return ;
    }
    if(ui->comboBox_2->currentIndex() == 0)
    {
        QMessageBox::information(this, QObject::tr("学位未选"), QObject::tr("请选择学位"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;
    query.prepare("insert into education_experience values(?, ?, ?, ?, ?, ?)");
    query.addBindValue(master_email);
    query.addBindValue(edu_lev);
    query.addBindValue(school);
    query.addBindValue(degree);
    query.addBindValue(start);
    query.addBindValue(end);
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("插入错误"), query.lastError().text());
        return ;
    }
    QMessageBox::critical(0, QObject::tr("添加教育经历成功"), QObject::tr("添加教育经历成功"), QMessageBox::Ok);
    this->hide();
    emit reload_education();
}

void experience_dialog::add_eduction_experience()
{
    this->show();
    ui->stackedWidget->setCurrentIndex(0);
}

void experience_dialog::add_job_experience()
{
    this->show();
    ui->stackedWidget->setCurrentIndex(1);
}

void experience_dialog::on_pushButton_2_clicked()
{
    this->close();
}
