#ifndef ATTENDANCESERVICE_H
#define ATTENDANCESERVICE_H

#include <QObject>
#include <QString>

class AttendanceService : public QObject
{
    Q_OBJECT

public:
    explicit AttendanceService(QObject *parent = nullptr);

    double calculateAttendanceRate(
        int present,
        int late,
        int excused,
        int totalSessions
        );

    void getAttendanceReport();

    void editAttendance(
        int recordId,
        QString newStatus,
        QString reason
        );

signals:
};

#endif // ATTENDANCESERVICE_H