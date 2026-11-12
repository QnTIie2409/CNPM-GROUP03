#include "mainwindow.h"
#include <QApplication>

#include <QSqlDatabase>
#include <QDebug>

#include "databasemanager.h"
#include "attendanceservice.h"
#include "timetableservice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << QSqlDatabase::drivers();

    DatabaseManager::connectDatabase();

    AttendanceService service;

    service.getAttendanceReport();

    service.editAttendance(
        2,
        "Excused",
        "Medical Certificate"
        );

    service.getAttendanceReport();

    TimetableService timetable;

    timetable.getLecturerTimetable();

    timetable.getStudentTimetable();

    timetable.addSchedule();

    timetable.updateSchedule();

    timetable.deleteSchedule();

    timetable.getLecturerTimetable();

    MainWindow w;
    w.show();

    return a.exec();
}