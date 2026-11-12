#include "timetableservice.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

TimetableService::TimetableService(QObject *parent)
    : QObject(parent)
{
}

void TimetableService::getLecturerTimetable()
{
    QSqlQuery query;

    if(query.exec("SELECT * FROM schedules"))
    {
        while(query.next())
        {
            qDebug()
            << "Schedule ID:" << query.value(0).toInt()
            << "Course ID:" << query.value(1).toInt()
            << "Type:" << query.value(2).toString()
            << "Day:" << query.value(3).toInt()
            << "Start:" << query.value(4).toString()
            << "End:" << query.value(5).toString()
            << "Room:" << query.value(6).toString();
        }
    }
    else
    {
        qDebug() << query.lastError().text();
    }
}

void TimetableService::getStudentTimetable()
{
    QSqlQuery query;

    if(query.exec("SELECT * FROM schedules"))
    {
        while(query.next())
        {
            qDebug()
            << "Schedule ID:" << query.value(0).toInt()
            << "Course ID:" << query.value(1).toInt()
            << "Room:" << query.value(6).toString();
        }
    }
    else
    {
        qDebug() << query.lastError().text();
    }
}

void TimetableService::addSchedule()
{
    QSqlQuery query;

    query.prepare(
        "INSERT INTO schedules "
        "(course_id,type,day_of_week,start_time,end_time,room,start_date,end_date) "
        "VALUES (1,'Class',3,'13:00:00','15:00:00','B201','2026-11-12','2027-01-30')"
        );

    if(query.exec())
    {
        qDebug() << "Schedule Added";
    }
    else
    {
        qDebug() << query.lastError().text();
    }
}

void TimetableService::updateSchedule()
{
    QSqlQuery query;

    query.prepare(
        "UPDATE schedules "
        "SET room='C301' "
        "WHERE id=1"
        );

    if(query.exec())
    {
        qDebug() << "Schedule Updated";
    }
    else
    {
        qDebug() << query.lastError().text();
    }
}

void TimetableService::deleteSchedule()
{
    QSqlQuery query;

    query.prepare(
        "DELETE FROM schedules "
        "WHERE id=2"
        );

    if(query.exec())
    {
        qDebug() << "Schedule Deleted";
    }
    else
    {
        qDebug() << query.lastError().text();
    }
}