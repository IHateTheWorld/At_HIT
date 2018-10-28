#include "homepage_dialog.h"
#include "ui_homepage_dialog.h"
#include <QMessageBox>
#include <QDebug>
#include "global_info.h"
#include <QtSql>
#include <QSqlQuery>
#include <QDateTime>

homepage_dialog::homepage_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::homepage_dialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("个人主页"));
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置只读
}

homepage_dialog::~homepage_dialog()
{
    delete ui;
}

void homepage_dialog::show_itself()
{
    //qDebug() << "master_email is: " << master_email;
    this->show();
    ui->stackedWidget->setCurrentIndex(0);
}

void homepage_dialog::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void homepage_dialog::on_pushButton_2_clicked()
{
    QSqlQuery query;
    query.prepare("select company, position, take_office, leave_office from job_experience where user = ?");
    query.addBindValue(master_email);
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("获取工作经历错误"), query.lastError().text());
        return ;
    }
    ui->tableWidget_2->setRowCount(query.size()); //这是需要注意的地方
    int line;
    line = 0;
    while(query.next())
    {
        ui->tableWidget_2->setItem(line, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget_2->setItem(line, 1, new QTableWidgetItem(query.value(1).toString()));

        QDateEdit * start_time= new QDateEdit();
        start_time->setDate(query.value(2).toDate());
        ui->tableWidget_2->setCellWidget(line, 2, start_time);
        QDateEdit * end_time= new QDateEdit();
        end_time->setDate(query.value(3).toDate());
        ui->tableWidget_2->setCellWidget(line, 3, end_time);

        line++;
    }
    ui->stackedWidget->setCurrentIndex(2);
}

void homepage_dialog::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void homepage_dialog::on_pushButton_6_clicked()
{
    QString date = ui->dateEdit_2->text();
    QString address = ui->lineEdit_6->text();
    QString password = ui->lineEdit_7->text();
    QString repeat_pwd = ui->lineEdit_8->text();

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

    QSqlQuery query;
    query.prepare("update user set born=?, address=?, password=? where email=?");
    query.addBindValue(date);
    query.addBindValue(address);
    query.addBindValue(password);
    query.addBindValue(master_email); //用户的注册邮箱
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("更新错误"), query.lastError().text());
        return ;
    }
    QMessageBox::critical(0, QObject::tr("更新成功"), QObject::tr("更新用户资料成功"), QMessageBox::Ok);
}

void homepage_dialog::on_pushButton_5_clicked()
{
    emit add_education_experience();
}

void homepage_dialog::on_pushButton_9_clicked()
{
    QSqlQuery query;
    query.prepare("select education_level, school, degree, start_date, end_date from education_experience where user = ?");
    query.addBindValue(master_email);
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("获取教育经历错误"), query.lastError().text());
        return ;
    }
    ui->tableWidget->setRowCount(query.size()); //这是需要注意的地方
    int line;
    line = 0;
    while(query.next())
    {
        QComboBox * edu_lev = new QComboBox();
        edu_lev->addItem(tr("请输入教育级别"));
        edu_lev->addItem(tr("儿童教育"));
        edu_lev->addItem(tr("小学教育"));
        edu_lev->addItem(tr("初中教育"));
        edu_lev->addItem(tr("高中教育"));
        edu_lev->addItem(tr("本科教育"));
        edu_lev->addItem(tr("硕士教育"));
        edu_lev->addItem(tr("博士教育"));
        edu_lev->setCurrentIndex(query.value(0).toInt());
        ui->tableWidget->setCellWidget(line, 0, edu_lev);

        QTableWidgetItem *item = new QTableWidgetItem(query.value(1).toString());
        ui->tableWidget->setItem(line, 1, item);

        QComboBox * degree = new QComboBox();
        degree->addItem(tr("请输入学位"));
        degree->addItem(tr("学士"));
        degree->addItem(tr("硕士"));
        degree->addItem(tr("博士"));
        if(query.value(2).toString() == tr("学士"))
            degree->setCurrentIndex(1);
        else if(query.value(2).toString() == tr("硕士"))
            degree->setCurrentIndex(2);
        else if(query.value(2).toString() == tr("博士"))
            degree->setCurrentIndex(3);
        ui->tableWidget->setCellWidget(line, 2, degree);

        QDateEdit * start_time= new QDateEdit();
        start_time->setDate(query.value(3).toDate());
        ui->tableWidget->setCellWidget(line, 3, start_time);
        QDateEdit * end_time= new QDateEdit();
        end_time->setDate(query.value(4).toDate());
        ui->tableWidget->setCellWidget(line, 4, end_time);

        line++;
    }
    ui->stackedWidget->setCurrentIndex(1);
}

void homepage_dialog::on_pushButton_7_clicked()
{
    int cur_line;

    cur_line = ui->tableWidget->currentRow();
    qDebug() << "cur_line: " << cur_line;

    QWidget * widget = ui->tableWidget->cellWidget(cur_line, 0);
    QComboBox * edu_lev = new QComboBox();
    edu_lev = (QComboBox*)widget;
    int edu_str = edu_lev->currentIndex();

    QString school = ui->tableWidget->item(cur_line, 1)->text();
    QWidget * dwidget = ui->tableWidget->cellWidget(cur_line, 2);
    QComboBox * dg = new QComboBox();
    dg = (QComboBox*)dwidget;
    QString degree = dg->currentText();

    qDebug() << master_email;
    qDebug() << edu_str;
    qDebug() << school;
    qDebug() << degree;

    QSqlQuery query;
    query.prepare("delete from education_experience where user=? and education_level=? and school=? and degree=?");
    query.addBindValue(master_email);
    query.addBindValue(edu_str);
    query.addBindValue(school);
    query.addBindValue(degree);

    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("删除教育经历错误"), query.lastError().text());
        return ;
    }
    QMessageBox::critical(0, QObject::tr("删除教育经历成功"), QObject::tr("删除教育经历成功"), QMessageBox::Ok);

    this->on_pushButton_9_clicked();
}

void homepage_dialog::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void homepage_dialog::on_pushButton_10_clicked() //保存日志
{
    QString log_text = ui->textEdit->toPlainText();

    qDebug() << log_text;

    QSqlQuery query;
    query.prepare("insert log (publish_date, publish_time, log_cont, publish_user) values(?, ?, ?, ?)");
    QDateTime current_time = QDateTime::currentDateTime();
    query.addBindValue(current_time);
    query.addBindValue(current_time);
    if(log_text.length() > 400)
    {
        QMessageBox::information(this, QObject::tr("日志过长"), QObject::tr("日志长度应该小于400字节"), QMessageBox::Ok);
        return ;
    }
    query.addBindValue(log_text);
    query.addBindValue(master_email);
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("发表日志错误"), query.lastError().text());
        return ;
    }
    QMessageBox::critical(0, QObject::tr("发表日志成功"), QObject::tr("发表日志成功"), QMessageBox::Ok);
}

void homepage_dialog::on_pushButton_11_clicked() //查看日志
{
    ;
}

void homepage_dialog::on_pushButton_14_clicked()
{
    emit add_job_experience();
}

void homepage_dialog::on_pushButton_15_clicked()
{
    QString name = ui->lineEdit->text();
    QString sex = "";
    bool type_switch;
    bool is_user_view;

    type_switch = ui->checkBox->isChecked();
    is_user_view = ui->checkBox_3->isChecked();

    if(type_switch && is_user_view)
    {
        QMessageBox::information(this, QObject::tr("查询冲突"), QObject::tr("嵌套或者使用视图最多选择一个"), QMessageBox::Ok);
        return ;
    }

    if(ui->checkBox_2->isChecked() == false)
    {
        if(ui->radioButton->isChecked())
            sex = "M";
        else if(ui->radioButton_2->isChecked())
            sex = "F";
    }

    QSqlQuery query;
    if(sex == "" && name != "")
    {
        if(type_switch)
            query.prepare("select email, name, sex, born, address from user, friends where friends.friend=user.email and name=? and friends.user=?");
        else if(is_user_view)
            query.prepare("select friend, name, sex, born, address from user_friends_view where friend=? and user=?");
        else
            query.prepare("select email, name, sex, born, address from user where name=? and email in (select friend from friends where user=?)");
        query.addBindValue(name);
        query.addBindValue(master_email);
    }
    else if(sex != "" && name == "")
    {
        if(type_switch)
            query.prepare("select email, name, sex, born, address from user, friends where friends.friend=user.email and sex=? and friends.user=?");
        else if(is_user_view)
            query.prepare("select friend, name, sex, born, address from user_friends_view where sex=? and user=?");
        else
            query.prepare("select email, name, sex, born, address from user where sex=? and email in (select friend from friends where user=?)");
        query.addBindValue(sex);
        query.addBindValue(master_email);
    }
    else if(sex != "" && name != "")
    {
        if(type_switch)
            query.prepare("select email, name, sex, born, address from user, friends where friends.friend=user.email and name=? and sex=? and friends.user=?");
        else if(is_user_view)
            query.prepare("select friend, name, sex, born, address from user_friends_view where friend=? and sex=? and user=?");
        else
            query.prepare("select email, name, sex, born, address from user where name=? and sex=? and email in (select friend from friends where user=?)");
        query.addBindValue(name);
        query.addBindValue(sex);
        query.addBindValue(master_email);
    }
    else
    {
        if(type_switch)
            query.prepare("select email, name, sex, born, address from user, friends where friends.friend=user.email and friends.user=?");
        else if(is_user_view)
            query.prepare("select friend, name, sex, born, address from user_friends_view where user=?");
        else
            query.prepare("select email, name, sex, born, address from user where email in (select friend from friends where user=?)");
        query.addBindValue(master_email);
    }
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("查询好友错误"), query.lastError().text());
        return ;
    }

    ui->tableWidget_3->setRowCount(query.size()); //这是需要注意的地方
    int line = 0;
    while(query.next())
    {
        ui->tableWidget_3->setItem(line, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget_3->setItem(line, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableWidget_3->setItem(line, 2, new QTableWidgetItem(query.value(2).toString()));
        QDateEdit * start_time= new QDateEdit();
        start_time->setDate(query.value(3).toDate());
        ui->tableWidget_3->setCellWidget(line, 3, start_time);
        ui->tableWidget_3->setItem(line, 4, new QTableWidgetItem(query.value(4).toString()));

        line++;
    }

    int total_friend = 0;
    QSqlQuery query2;
    query2.prepare("select count(friend) from friends where user=? group by user"); //通过分组查询来获取好友的数量
    query2.addBindValue(master_email);
    if(!query2.exec())
    {
        QMessageBox::critical(0, QObject::tr("获取查询结果错误"), query2.lastError().text());
        return ;
    }
    if(query2.next())
        total_friend = query2.value(0).toInt();
    ui->label_8->setText(QString("查询结果：%0/%1").arg(query.size()).arg(total_friend));
}

void homepage_dialog::on_pushButton_16_clicked()
{
    QString name = ui->lineEdit->text();

    if(name == NULL)
    {
        QMessageBox::information(this, QObject::tr("姓名为空"), QObject::tr("请输入需要查找的姓名"), QMessageBox::Ok);
        return ;
    }

    QSqlQuery query;
    query.prepare("select email, name, sex, born, address from user where name=?");
    query.addBindValue(name);
    if(!query.exec())
    {
        QMessageBox::critical(0, QObject::tr("获取查询结果错误"), query.lastError().text());
        return ;
    }
    int line = 0;
    ui->tableWidget_3->setRowCount(query.size());
    while(query.next())
    {
        ui->tableWidget_3->setItem(line, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget_3->setItem(line, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableWidget_3->setItem(line, 2, new QTableWidgetItem(query.value(2).toString()));
        QDateEdit * start_time= new QDateEdit();
        start_time->setDate(query.value(3).toDate());
        ui->tableWidget_3->setCellWidget(line, 3, start_time);
        ui->tableWidget_3->setItem(line, 4, new QTableWidgetItem(query.value(4).toString()));

        line++;
    }
}
