#ifndef ATTENDANCECHANGELOG_H
#define ATTENDANCECHANGELOG_H

#include "AttendanceRecord.h"
#include <QDateTime>
#include <QString>

// Một dòng trong bảng Audit Log (lịch sử giảng viên sửa điểm danh).
struct AttendanceChangeLog {
    int logId = 0;
    int recordId = 0;
    AttendanceStatus oldStatus = AttendanceStatus::ABSENT;
    AttendanceStatus newStatus = AttendanceStatus::ABSENT;
    QString reason;
    QDateTime changedAt;

    int lecturerUserId = 0;     // ai sửa
    QString lecturerName;

    int studentUserId = 0;      // sửa cho sinh viên nào
    QString studentCode;
    QString studentName;

    int sessionId = 0;          // thuộc phiên / môn nào
    QDateTime sessionStart;
    int courseId = 0;
    QString courseCode;
};

#endif // ATTENDANCECHANGELOG_H
