#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

struct ServiceResult
{
    bool ok = false;
    QString message;
    qint64 id = 0;

    static ServiceResult success(const QString &msg = QString(), qint64 newId = 0)
    {
        ServiceResult r;
        r.ok = true;
        r.message = msg;
        r.id = newId;
        return r;
    }

    static ServiceResult failure(const QString &msg)
    {
        ServiceResult r;
        r.ok = false;
        r.message = msg;
        return r;
    }
};

inline QSqlDatabase serviceDb()
{
    if (QSqlDatabase::contains("qt_mysql_main")) {
        return QSqlDatabase::database("qt_mysql_main");
    }
    return QSqlDatabase::database();
}

inline bool isDuplicateKeyError(const QSqlError &e)
{
    return e.nativeErrorCode() == QLatin1String("1062")
           || e.text().contains(QLatin1String("Duplicate entry"), Qt::CaseInsensitive);
}

inline bool isForeignKeyError(const QSqlError &e)
{
    return e.nativeErrorCode() == QLatin1String("1451")
           || e.nativeErrorCode() == QLatin1String("1452")
           || e.text().contains(QLatin1String("foreign key"), Qt::CaseInsensitive);
}
