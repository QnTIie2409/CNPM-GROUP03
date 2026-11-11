#ifndef ATTENDANCERECORD_H
#define ATTENDANCERECORD_H

#include <QString>
#include <QDateTime>

enum class AttendanceStatus {
    PRESENT,
    LATE,
    ABSENT,
    EXCUSED
};

struct AttendanceRecord {
    int recordId = 0;
    int sessionId = 0;
    int studentUserId = 0;
    QString studentCode;
    QString studentName;
    AttendanceStatus status = AttendanceStatus::ABSENT;
    QDateTime checkInTime;

    static QString statusToString(AttendanceStatus s) {
        switch (s) {
            case AttendanceStatus::PRESENT: return "PRESENT";
            case AttendanceStatus::LATE: return "LATE";
            case AttendanceStatus::ABSENT: return "ABSENT";
            case AttendanceStatus::EXCUSED: return "EXCUSED";
        }
        return "ABSENT";
    }

    static AttendanceStatus stringToStatus(const QString& str) {
        if (str == "PRESENT") return AttendanceStatus::PRESENT;
        if (str == "LATE") return AttendanceStatus::LATE;
        if (str == "EXCUSED") return AttendanceStatus::EXCUSED;
        return AttendanceStatus::ABSENT;
    }
};

#endif // ATTENDANCERECORD_H
