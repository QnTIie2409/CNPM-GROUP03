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

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT session_id FROM attendance_sessions WHERE course_id = ? AND status = 'OPEN' LIMIT 1");
    checkQuery.addBindValue(courseId);
    if (checkQuery.exec() && checkQuery.next()) {
        return ServiceResult::failure("An attendance session is already open for this course!");
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

    m_activeSessionId = newSessionId;
    if (!m_timer->isActive())
        m_timer->start();

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

    if (m_activeSessionId == sessionId) {
        m_timer->stop();
        m_activeSessionId = -1;
    }

    emit sessionClosed(sessionId);

    QString msg = QString("Attendance session closed successfully! Automatically marked %1 student(s) as Absent.")
                      .arg(absentCount);

    return ServiceResult::success(msg, sessionId);
}

std::optional<AttendanceSession> SessionService::getOpenSession(int courseId) {
    return m_sessionRepo.findOpenSessionForCourse(courseId);
}

std::optional<AttendanceSession> SessionService::getSessionById(int sessionId) {
    return m_sessionRepo.findById(sessionId);
}

void SessionService::onRotateCode() {
    const auto openSessions = m_sessionRepo.findOpenSessions();
    if (openSessions.empty()) {
        m_timer->stop();
        m_activeSessionId = -1;
        return;
    }

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
            const QString newCode = generateRandomCode();
            if (m_sessionRepo.updateSessionCode(session.sessionId, newCode))
                emit codeRotated(session.sessionId, newCode);
        }
    }

    if (m_sessionRepo.findOpenSessions().empty()) {
        m_timer->stop();
        m_activeSessionId = -1;
    }
}
