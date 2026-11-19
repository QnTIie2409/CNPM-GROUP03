#ifndef ATTENDANCEREPOSITORY_H
#define ATTENDANCEREPOSITORY_H

#include "../models/AttendanceRecord.h"
#include <optional>
#include <vector>

struct AttendanceChangeLogItem {
    int logId = 0;
    int recordId = 0;
    QString studentCode;
    QString studentName;
    QString oldStatus;
    QString newStatus;
    QString reason;
    QDateTime changedAt;
    QString lecturerName;
};

class AttendanceRepository {
public:
    std::optional<AttendanceRecord> findRecord(int sessionId, int studentUserId);
    std::vector<AttendanceRecord> findBySession(int sessionId);
    std::vector<AttendanceRecord> findByStudentAndCourse(int studentUserId, int courseId);
    std::vector<AttendanceChangeLogItem> findLogsBySession(int sessionId);

    bool saveCheckIn(int sessionId, int studentUserId, AttendanceStatus status);
    bool markAbsentForUnchecked(int sessionId, int courseId);
    bool updateStatusWithLog(int recordId, AttendanceStatus newStatus, int lecturerUserId, const QString& reason);
};

#endif
