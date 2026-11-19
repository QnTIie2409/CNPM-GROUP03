#include "SessionService.h"
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "ServiceCommon.h"

SessionService::SessionService() {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SessionService::onRotateCode);
    m_timer->setInterval(1000);
    // Keep monitoring sessions created before restart or by another app.
    m_timer->start();
}

SessionService& SessionService::instance() {
    static SessionService inst;
    return inst;
}

QString SessionService::generateRandomCode() {
    int code = QRandomGenerator::global()->bounded(100000, 1000000);
    return QString::number(code);
}

ServiceResult SessionService::openSession(int courseId, int lecturerUserId, int durationMin, int presentMin, int lateMin) 
{
    if (presentMin <= 0 || presentMin > lateMin || lateMin > durationMin) {
        return ServiceResult::failure("Invalid time window configuration: must satisfy 0 < OnTime <= Late <= TotalDuration.");
    }

    QSqlDatabase db = serviceDb();
    if (!db.isOpen()) {
        return ServiceResult::failure("Cannot connect to database.");
    }

    if (!m_sessionRepo.lecturerTeachesCourse(lecturerUserId, courseId)) {
        return ServiceResult::failure("This Lecturer is not assigned to the selected course.");
    }

    const auto existing = m_sessionRepo.findOpenSessionForCourse(courseId);
    if (existing.has_value()) {
        const qint64 elapsed = m_sessionRepo.serverSecondsSince(existing->startTime);
        if (elapsed >= 0 && elapsed >= static_cast<qint64>(existing->durationMinutes) * 60) {
            const auto closed = closeSession(existing->sessionId);
            if (!closed.ok)
                return closed;
        } else {
            return ServiceResult::failure("An attendance session is already open for this course!");
        }
    }
    QString code = generateRandomCode();

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO attendance_sessions (course_id, user_id, start_time, duration_minutes, "
                        "present_window_minutes, late_window_minutes, session_code, code_generated_at, status) "
                        "VALUES (?, ?, NOW(), ?, ?, ?, ?, NOW(), 'OPEN')");
    insertQuery.addBindValue(courseId);
    insertQuery.addBindValue(lecturerUserId);
    insertQuery.addBindValue(durationMin);
    insertQuery.addBindValue(presentMin);
    insertQuery.addBindValue(lateMin);
    insertQuery.addBindValue(code);

    if (!insertQuery.exec()) {
        qWarning() << "openSession failed:" << insertQuery.lastError().text();
        return ServiceResult::failure("Failed to create new attendance session in database.");
    }

    qint64 newSessionId = 0;
    QSqlQuery idQuery(db);
    if (idQuery.exec("SELECT LAST_INSERT_ID()") && idQuery.next()) {
        newSessionId = idQuery.value(0).toLongLong();
    }

    emit sessionOpened(static_cast<int>(newSessionId));

    return ServiceResult::success("Attendance session opened successfully! Initial code: " + code, newSessionId);
}

ServiceResult SessionService::closeSession(int sessionId) 
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen()) {
        return ServiceResult::failure("Cannot connect to database.");
    }

    auto sOpt = m_sessionRepo.findById(sessionId);
    if (!sOpt.has_value()) {
        return ServiceResult::failure("Attendance session not found.");
    }

    if (sOpt->status == SessionStatus::CLOSED) {
        return ServiceResult::failure("This attendance session has already been closed.");
    }

    db.transaction();

    QSqlQuery closeQuery(db);
    closeQuery.prepare("UPDATE attendance_sessions SET status = 'CLOSED', closed_at = NOW() WHERE session_id = ?");
    closeQuery.addBindValue(sessionId);
    if (!closeQuery.exec()) {
        db.rollback();
        qWarning() << "closeSession failed:" << closeQuery.lastError().text();
        return ServiceResult::failure("Failed to update session status to CLOSED.");
    }

    QSqlQuery absentQuery(db);
    absentQuery.prepare(
        "INSERT INTO attendance_records (session_id, user_id, status, check_in_time) "
        "SELECT ?, e.user_id, 'ABSENT', NULL "
        "FROM enrollments e "
        "WHERE e.course_id = ? "
        "AND e.user_id NOT IN (SELECT user_id FROM attendance_records WHERE session_id = ?)"
    );
    absentQuery.addBindValue(sessionId);
    absentQuery.addBindValue(sOpt->courseId);
    absentQuery.addBindValue(sessionId);

    int absentCount = 0;
    if (absentQuery.exec()) {
        absentCount = absentQuery.numRowsAffected(); 
    } else {
        db.rollback();
        qWarning() << "Auto-mark absent failed:" << absentQuery.lastError().text();
        return ServiceResult::failure("Failed to automatically mark absent for remaining students.");
    }

    if (!db.commit()) {
        return ServiceResult::failure("Failed to commit attendance session closure.");
    }

    emit sessionClosed(sessionId);

    QString msg = QString("Attendance session closed successfully! Automatically marked %1 student(s) as Absent.")
                      .arg(absentCount);

    return ServiceResult::success(msg, sessionId);
}

std::optional<AttendanceSession> SessionService::getOpenSession(int courseId) {
    const auto session = m_sessionRepo.findOpenSessionForCourse(courseId);
    if (!session.has_value())
        return std::nullopt;

    const qint64 elapsed = m_sessionRepo.serverSecondsSince(session->startTime);
    if (elapsed >= 0 && elapsed >= static_cast<qint64>(session->durationMinutes) * 60) {
        closeSession(session->sessionId);
        return std::nullopt;
    }
    return session;
}

std::optional<AttendanceSession> SessionService::getSessionById(int sessionId) {
    return m_sessionRepo.findById(sessionId);
}

void SessionService::onRotateCode() {
    const auto openSessions = m_sessionRepo.findOpenSessions();

    for (const auto &session : openSessions) {
        const qint64 elapsedSeconds = m_sessionRepo.serverSecondsSince(session.startTime);
        if (elapsedSeconds < 0)
            continue;
        if (elapsedSeconds >= static_cast<qint64>(session.durationMinutes) * 60) {
            closeSession(session.sessionId);
            continue;
        }

        const qint64 codeAge = m_sessionRepo.serverSecondsSince(session.codeGeneratedAt);
        if (codeAge < 0)
            continue;
        if (codeAge >= 60) {
            QString newCode;
            do {
                newCode = generateRandomCode();
            } while (newCode == session.sessionCode);
            if (m_sessionRepo.updateSessionCode(session.sessionId, newCode))
                emit codeRotated(session.sessionId, newCode);
        }
    }

}
