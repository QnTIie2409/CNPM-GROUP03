#include "AttendanceService.h"

AttendanceService& AttendanceService::instance() {
    static AttendanceService inst;
    return inst;
}

CourseAttendanceStats AttendanceService::getStudentStats(int studentUserId, int courseId) {
    CourseAttendanceStats stats;
    auto records = m_attendanceRepo.findByStudentAndCourse(studentUserId, courseId);
    stats.totalCompletedSessions = records.size();

    for (const auto& r : records) {
        switch (r.status) {
            case AttendanceStatus::PRESENT: stats.presentCount++; break;
            case AttendanceStatus::LATE: stats.lateCount++; break;
            case AttendanceStatus::EXCUSED: stats.excusedCount++; break;
            case AttendanceStatus::ABSENT: stats.absentCount++; break;
        }
    }

    if (stats.totalCompletedSessions > 0) {
        stats.attendanceRate = ((double)(stats.presentCount + stats.lateCount + stats.excusedCount) / stats.totalCompletedSessions) * 100.0;
    }
    return stats;
}

std::vector<AttendanceRecord> AttendanceService::getSessionRecords(int sessionId) {
    return m_attendanceRepo.findBySession(sessionId);
}

bool AttendanceService::editRecordStatus(int recordId, AttendanceStatus newStatus, int lecturerUserId, const QString& reason) {
    return m_attendanceRepo.updateStatusWithLog(recordId, newStatus, lecturerUserId, reason);
}
