#ifndef TIMETABLESERVICE_H
#define TIMETABLESERVICE_H

#include <QObject>

class TimetableService : public QObject
{
    Q_OBJECT

public:
    explicit TimetableService(QObject *parent = nullptr);

    void getLecturerTimetable();

    void getStudentTimetable();

    void addSchedule();

    void updateSchedule();

    void deleteSchedule();

signals:
};

#endif // TIMETABLESERVICE_H