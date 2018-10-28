#include "register_dialog.h"
#include "ui_register_dialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QtSql>

register_dialog::register_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::register_dialog)
{
    ui->setupUi(this);
}

register_dialog::~register_dialog()
{
    delete ui;
}

void register_dialog::on_pushButton_clicked()
{
    QString date = ui->dateEdit->text();
    QString email = ui->lineEdit->text();
    QString name = ui->lineEdit_2->text();
    QString password = ui->lineEdit_3->text();
    QString repeat_pwd = ui->lineEdit_4->text();
    QString address = ui->lineEdit_5->text();
    bool is_M = ui->radioButton->isChecked();
    bool is_F = ui->radioButton_2->isChecked();

    QSqlQuery query;

    if(email == NULL)
    {
        QMessageBox::information(this, QObject::tr("邮箱为空"), QObject::tr("请输入邮箱"), QMessageBox::Ok);
        return ;
    }
    else
    {
        query.exec(QString("select * from user where email = '%1'").arg(email)); //注意email对应的应该是字符串
        if(query.size())
        {
            QMessageBox::information(this, QObject::tr("邮箱已被注册"), QObject::tr("请重新输入邮箱"), QMessageBox::Ok);
            return ;
        }
    }
    if(name == NULL)
    {
        QMessageBox::information(this, QObject::tr("姓名为空"), QObject::tr("请输入姓名"), QMessageBox::Ok);
        return ;
    }
    if(!is_M && !is_F)
    {
        QMessageBox::information(this, QObject::tr("性别为空"), QObject::tr("请选择性别"), QMessageBox::Ok);
        return ;
    }
    if(password == NULL)
    {
        QMessageBox::information(this, QObject::tr("密码为空"), QObject::tr("请输入密码"), QMessageBox::Ok);
        return ;
    }
    if(password != repeat_pwd)
    {
        QMessageBox::information(this, QObject::tr("密码不一致"), QObject::tr("请重新确认密码"), QMessageBox::Ok);
        return ;
    }

    query.prepare("insert into user values(?, ?, ?, ?, ?, ?)");
    query.addBindValue(email);
    query.addBindValue(name);
    if(is_M)
        query.addBindValue("M");
    else
        query.addBindValue("F");
    query.addBindValue(date);
    query.addBindValue(address);
    query.addBindValue(password);
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("插入错误"), query.lastError().text());
        return ;
    }
    QMessageBox::critical(0, QObject::tr("添加新用户成功"), QObject::tr("添加新用户成功"), QMessageBox::Ok);
    this->hide();
    emit show_login();
}

void register_dialog::show_itself()
{
    this->show();
}

void register_dialog::on_pushButton_2_clicked()
{
    this->hide();
    emit show_login();
}
