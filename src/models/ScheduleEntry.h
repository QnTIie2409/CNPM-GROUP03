#ifndef SCHEDULEENTRY_H
#define SCHEDULEENTRY_H

#include <QString>
#include <QTime>
#include <QDate>

enum class ScheduleType {
    CLASS,
    STUDY
};

struct ScheduleEntry {
    int scheduleId = 0;
    int courseId = 0;
    QString courseCode;
    QString courseName;
    ScheduleType type = ScheduleType::CLASS;
    int dayOfWeek = 1;
    QTime startTime;
    QTime endTime;
    QString room;
    QDate startDate;
    QDate endDate;
};

#endif
