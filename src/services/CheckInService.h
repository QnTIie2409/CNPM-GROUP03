#ifndef CHECKINSERVICE_H
#define CHECKINSERVICE_H

#include "../models/AttendanceRecord.h"
#include "../repositories/SessionRepository.h"
#include "../repositories/CourseRepository.h"
#include "../repositories/AttendanceRepository.h"
#include "ServiceCommon.h"
#include <QString>

enum class CheckInResultCode {
    SUCCESS_PRESENT,
    SUCCESS_LATE,
    ERROR_SESSION_CLOSED,
    ERROR_WRONG_CODE,
    ERROR_NOT_ENROLLED,
    ERROR_ALREADY_CHECKED_IN,
    ERROR_SYSTEM
};

struct CheckInResponse {
    CheckInResultCode code;
    QString message;
    AttendanceStatus status = AttendanceStatus::ABSENT;
    QDateTime checkInTime;
};

class CheckInService {
public:
    static CheckInService& instance();

    CheckInResponse submitCheckIn(int sessionId, int studentUserId, const QString& enteredCode);

private:
    CheckInService() = default;
    SessionRepository m_sessionRepo;
    CourseRepository m_courseRepo;
    AttendanceRepository m_attendanceRepo;
};

#endif // CHECKINSERVICE_H
