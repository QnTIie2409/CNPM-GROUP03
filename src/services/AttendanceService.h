#ifndef ATTENDANCESERVICE_H
#define ATTENDANCESERVICE_H

#include "../models/AttendanceRecord.h"
#include "../repositories/AttendanceRepository.h"
#include <vector>

struct CourseAttendanceStats {
    int totalCompletedSessions = 0;
    int presentCount = 0;
    int lateCount = 0;
    int excusedCount = 0;
    int absentCount = 0;
    double attendanceRate = 0.0;
};

class AttendanceService {
public:
    static AttendanceService& instance();

    CourseAttendanceStats getStudentStats(int studentUserId, int courseId);
    std::vector<AttendanceRecord> getSessionRecords(int sessionId);
    bool editRecordStatus(int recordId, AttendanceStatus newStatus, int lecturerUserId, const QString& reason);

private:
    AttendanceService() = default;
    AttendanceRepository m_attendanceRepo;
};

#endif
