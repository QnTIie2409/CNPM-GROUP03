#include "databasemanager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QString>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

bool DatabaseManager::connectDatabase()
{
    QSqlDatabase db =
        QSqlDatabase::addDatabase("QODBC");

    QString dsn =
        "Driver={MySQL ODBC 26.10 Unicode Driver};"
        "Server=localhost;"
        "Database=student_attendance_system;"
        "User=root;"
        "Password=thanh123@;"
        "Option=3;";

    db.setDatabaseName(dsn);

    if (db.open())
    {
        qDebug() << "Database Connected!";
        return true;
    }

    qDebug() << db.lastError().text();
    qDebug() << "Database Failed!";

    return false;
}