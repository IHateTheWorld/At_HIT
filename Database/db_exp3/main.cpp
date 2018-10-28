#include <QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include "connect_db.h"
#include "login_dialog.h"
#include "register_dialog.h"
#include "homepage_dialog.h"
#include "global_info.h"
#include "experience_dialog.h"

QString master_email = ""; //全局变量

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    //连接数据库
    if(!connect_db())
        return 0;

    login_dialog login_ui;
    register_dialog register_ui;
    homepage_dialog homepage_ui;
    experience_dialog experience_ui;

    login_ui.show();

    //信号与槽之间关联
    QObject::connect(&login_ui, SIGNAL(show_register()), &register_ui, SLOT(show_itself()));
    QObject::connect(&register_ui, SIGNAL(show_login()), &login_ui, SLOT(show_itself()));
    QObject::connect(&login_ui, SIGNAL(show_homepage()), &homepage_ui, SLOT(show_itself()));
    QObject::connect(&homepage_ui, SIGNAL(add_education_experience()), &experience_ui, SLOT(add_eduction_experience()));
    QObject::connect(&homepage_ui, SIGNAL(add_job_experience()), &experience_ui, SLOT(add_job_experience()));
    QObject::connect(&experience_ui, SIGNAL(reload_education()), &homepage_ui, SLOT(on_pushButton_9_clicked()));

    return a.exec();
}
