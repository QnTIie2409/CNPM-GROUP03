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
        resp.message = "PhiĂªn Ä‘iá»ƒm danh Ä‘Ă£ Ä‘Ă³ng hoáº·c khĂ´ng tá»“n táº¡i.";
        return resp;
    }

    const AttendanceSession& session = sOpt.value();

    if (!m_courseRepo.isStudentEnrolled(studentUserId, session.courseId)) {
        resp.code = CheckInResultCode::ERROR_NOT_ENROLLED;
        resp.message = "Báº¡n khĂ´ng cĂ³ tĂªn trong danh sĂ¡ch ghi danh mĂ´n há»c nĂ y.";
        return resp;
    }

    if (m_attendanceRepo.findRecord(sessionId, studentUserId).has_value()) {
        resp.code = CheckInResultCode::ERROR_ALREADY_CHECKED_IN;
        resp.message = "Báº¡n Ä‘Ă£ hoĂ n thĂ nh Ä‘iá»ƒm danh cho phiĂªn nĂ y rá»“i.";
        return resp;
    }

    if (enteredCode.trimmed() != session.sessionCode.trimmed()) {
        resp.code = CheckInResultCode::ERROR_WRONG_CODE;
        resp.message = "MĂ£ Ä‘iá»ƒm danh khĂ´ng chĂ­nh xĂ¡c hoáº·c Ä‘Ă£ háº¿t háº¡n (xoay mĂ£ 60s).";
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
        resp.message = "Thá»i gian Ä‘iá»ƒm danh cho phiĂªn nĂ y Ä‘Ă£ káº¿t thĂºc.";
        return resp;
    }

    AttendanceStatus status = (elapsedMin <= session.presentWindowMinutes) ? AttendanceStatus::PRESENT : AttendanceStatus::LATE;

    if (m_attendanceRepo.saveCheckIn(sessionId, studentUserId, status)) {
        resp.status = status;
        resp.checkInTime = serverNow;
        if (status == AttendanceStatus::PRESENT) {
            resp.code = CheckInResultCode::SUCCESS_PRESENT;
            resp.message = "Äiá»ƒm danh THĂ€NH CĂ”NG (ÄĂºng giá» - Present)!";
        } else {
            resp.code = CheckInResultCode::SUCCESS_LATE;
            resp.message = "Äiá»ƒm danh THĂ€NH CĂ”NG (Äi muá»™n - Late)!";
        }
    } else {
        resp.code = CheckInResultCode::ERROR_SYSTEM;
        resp.message = "CĂ³ lá»—i xáº£y ra khi lÆ°u báº£n ghi Ä‘iá»ƒm danh.";
    }

    return resp;
}
