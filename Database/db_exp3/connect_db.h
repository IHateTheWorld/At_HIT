#ifndef CONNECT_DB_H
#define CONNECT_DB_H

#include <QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QStringList>
#include <QMessageBox>

static bool connect_db()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    db.setHostName("localhost");
    db.setDatabaseName("social_network");
    db.setUserName("root");
    db.setPassword("456567lw");
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("连接数据库失败"), db.lastError().text());
        return false;
    }
    //qDebug() << "Succeeded to connect to root mysql admin";
    return true;
}

#endif // CONNECT_DB_H
