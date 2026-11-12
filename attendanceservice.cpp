#include "attendanceservice.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

AttendanceService::AttendanceService(QObject *parent)
    : QObject(parent)
{
}

double AttendanceService::calculateAttendanceRate(
    int present,
    int late,
    int excused,
    int totalSessions)
{
    if (totalSessions == 0)
        return 0;

    return (double)(present + late + excused)
           * 100.0
           / totalSessions;
}

void AttendanceService::getAttendanceReport()
{
    QSqlQuery query;

    if(query.exec("SELECT * FROM attendance_records"))
    {
        while(query.next())
        {
            qDebug()
            << "Record ID:" << query.value(0).toInt()
            << "Student ID:" << query.value(1).toInt()
            << "Session ID:" << query.value(2).toInt()
            << "Status:" << query.value(3).toString();
        }
    }
    else
    {
        qDebug() << query.lastError().text();
    }
}

void AttendanceService::editAttendance(
    int recordId,
    QString newStatus,
    QString reason)
{
    QSqlQuery query;

    query.prepare(
        "SELECT status "
        "FROM attendance_records "
        "WHERE id = ?"
        );

    query.addBindValue(recordId);

    if(!query.exec() || !query.next())
    {
        qDebug() << "Record not found";
        return;
    }

    QString oldStatus =
        query.value(0).toString();

    QSqlQuery updateQuery;

    updateQuery.prepare(
        "UPDATE attendance_records "
        "SET status = ? "
        "WHERE id = ?"
        );

    updateQuery.addBindValue(newStatus);
    updateQuery.addBindValue(recordId);

    if(updateQuery.exec())
    {
        qDebug() << "Attendance Updated";
    }

    QSqlQuery logQuery;

    logQuery.prepare(
        "INSERT INTO attendance_change_logs "
        "(attendance_record_id,"
        "lecturer_id,"
        "old_status,"
        "new_status,"
        "reason,"
        "changed_at)"
        "VALUES(?,?,?,?,?,NOW())"
        );

    logQuery.addBindValue(recordId);
    logQuery.addBindValue(1);
    logQuery.addBindValue(oldStatus);
    logQuery.addBindValue(newStatus);
    logQuery.addBindValue(reason);

    if(logQuery.exec())
    {
        qDebug() << "Log Saved";
    }
    else
    {
        qDebug() << logQuery.lastError().text();
    }
}