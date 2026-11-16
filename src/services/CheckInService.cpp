#include "CheckInService.h"
#include "ServiceCommon.h"
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

CheckInService& CheckInService::instance() {
    static CheckInService inst;
    return inst;
}

CheckInResponse CheckInService::submitCheckIn(int sessionId, int studentUserId, const QString& enteredCode) {
    CheckInResponse resp;

    auto sOpt = m_sessionRepo.findById(sessionId);
    if (!sOpt.has_value() || sOpt->status != SessionStatus::OPEN) {
        resp.code = CheckInResultCode::ERROR_SESSION_CLOSED;
        resp.message = "Attendance session is closed or does not exist.";
        return resp;
    }

    const AttendanceSession& session = sOpt.value();

    if (!m_courseRepo.isStudentEnrolled(studentUserId, session.courseId)) {
        resp.code = CheckInResultCode::ERROR_NOT_ENROLLED;
        resp.message = "You are not enrolled in this course.";
        return resp;
    }

    if (m_attendanceRepo.findRecord(sessionId, studentUserId).has_value()) {
        resp.code = CheckInResultCode::ERROR_ALREADY_CHECKED_IN;
        resp.message = "You have already checked in for this session.";
        return resp;
    }

    if (enteredCode.trimmed() != session.sessionCode.trimmed()) {
        resp.code = CheckInResultCode::ERROR_WRONG_CODE;
        resp.message = "Check-in code is incorrect or expired (codes rotate every 60s).";
        return resp;
    }

    QSqlDatabase db = serviceDb();
    QSqlQuery timeQuery(db);
    timeQuery.prepare("SELECT TIMESTAMPDIFF(MINUTE, start_time, NOW()), NOW() FROM attendance_sessions WHERE session_id = ?");
    timeQuery.addBindValue(sessionId);

    qint64 elapsedMin = 0;
    QDateTime serverNow;
    if (timeQuery.exec() && timeQuery.next()) {
        elapsedMin = timeQuery.value(0).toLongLong();
        serverNow = timeQuery.value(1).toDateTime();
    } else {
        serverNow = QDateTime::currentDateTime();
        elapsedMin = session.startTime.secsTo(serverNow) / 60;
    }

    if (elapsedMin > session.lateWindowMinutes || elapsedMin > session.durationMinutes) {
        resp.code = CheckInResultCode::ERROR_SESSION_CLOSED;
        resp.message = "Check-in time window for this session has ended.";
        return resp;
    }

    AttendanceStatus status = (elapsedMin <= session.presentWindowMinutes) ? AttendanceStatus::PRESENT : AttendanceStatus::LATE;

    if (m_attendanceRepo.saveCheckIn(sessionId, studentUserId, status)) {
        resp.status = status;
        resp.checkInTime = serverNow;
        if (status == AttendanceStatus::PRESENT) {
            resp.code = CheckInResultCode::SUCCESS_PRESENT;
            resp.message = "Check-in SUCCESSFUL (On Time - Present)!";
        } else {
            resp.code = CheckInResultCode::SUCCESS_LATE;
            resp.message = "Check-in SUCCESSFUL (Late)!";
        }
    } else {
        resp.code = CheckInResultCode::ERROR_SYSTEM;
        resp.message = "An error occurred while saving check-in record.";
    }

    return resp;
}
