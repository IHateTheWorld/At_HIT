#include "login_dialog.h"
#include "ui_login_dialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include "global_info.h"

login_dialog::login_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login_dialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("登录"));
}

login_dialog::~login_dialog()
{
    delete ui;
}

void login_dialog::on_pushButton_2_clicked()
{
   QSqlQuery query;

   query.exec("select email, password from user");
   while(query.next())
   {
       if(query.value(0).toString() == ui->lineEdit->text() && query.value(1).toString() == ui->lineEdit_2->text())
       {
           //qDebug() << "登录成功";
           master_email = query.value(0).toString();
           this->hide();
           emit show_homepage();
           return ;
       }
   }
   QMessageBox::information(this, QObject::tr("登录失败"), QObject::tr("请确认密码是否与用户匹配"), QMessageBox::Ok);
}

void login_dialog::on_pushButton_clicked()
{
    this->hide();
    emit show_register();
}

void login_dialog::show_itself()
{
    this->show();
}
