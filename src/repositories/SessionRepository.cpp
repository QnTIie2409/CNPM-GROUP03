#include "SessionRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>

std::optional<AttendanceSession> SessionRepository::findById(int sessionId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT session_id, course_id, user_id, start_time, duration_minutes, "
                  "present_window_minutes, late_window_minutes, session_code, code_generated_at, status, closed_at "
                  "FROM attendance_sessions WHERE session_id = :id");
    query.bindValue(":id", sessionId);

    if (query.exec() && query.next()) {
        AttendanceSession s;
        s.sessionId = query.value("session_id").toInt();
        s.courseId = query.value("course_id").toInt();
        s.lecturerUserId = query.value("user_id").toInt();
        s.startTime = query.value("start_time").toDateTime();
        s.durationMinutes = query.value("duration_minutes").toInt();
        s.presentWindowMinutes = query.value("present_window_minutes").toInt();
        s.lateWindowMinutes = query.value("late_window_minutes").toInt();
        s.sessionCode = query.value("session_code").toString();
        s.codeGeneratedAt = query.value("code_generated_at").toDateTime();
        s.status = (query.value("status").toString() == "OPEN") ? SessionStatus::OPEN : SessionStatus::CLOSED;
        s.closedAt = query.value("closed_at").toDateTime();
        return s;
    }
    return std::nullopt;
}

std::optional<AttendanceSession> SessionRepository::findOpenSessionForCourse(int courseId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT session_id, course_id, user_id, start_time, duration_minutes, "
                  "present_window_minutes, late_window_minutes, session_code, code_generated_at, status, closed_at "
                  "FROM attendance_sessions WHERE course_id = :cid AND status = 'OPEN' ORDER BY start_time DESC LIMIT 1");
    query.bindValue(":cid", courseId);

    if (query.exec() && query.next()) {
        AttendanceSession s;
        s.sessionId = query.value("session_id").toInt();
        s.courseId = query.value("course_id").toInt();
        s.lecturerUserId = query.value("user_id").toInt();
        s.startTime = query.value("start_time").toDateTime();
        s.durationMinutes = query.value("duration_minutes").toInt();
        s.presentWindowMinutes = query.value("present_window_minutes").toInt();
        s.lateWindowMinutes = query.value("late_window_minutes").toInt();
        s.sessionCode = query.value("session_code").toString();
        s.codeGeneratedAt = query.value("code_generated_at").toDateTime();
        s.status = SessionStatus::OPEN;
        return s;
    }
    return std::nullopt;
}

std::vector<AttendanceSession> SessionRepository::findByCourse(int courseId) {
    std::vector<AttendanceSession> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT session_id, course_id, user_id, start_time, duration_minutes, "
                  "present_window_minutes, late_window_minutes, session_code, code_generated_at, status, closed_at "
                  "FROM attendance_sessions WHERE course_id = :cid ORDER BY start_time DESC");
    query.bindValue(":cid", courseId);
    if (query.exec()) {
        while (query.next()) {
            AttendanceSession s;
            s.sessionId = query.value("session_id").toInt();
            s.courseId = query.value("course_id").toInt();
            s.lecturerUserId = query.value("user_id").toInt();
            s.startTime = query.value("start_time").toDateTime();
            s.durationMinutes = query.value("duration_minutes").toInt();
            s.presentWindowMinutes = query.value("present_window_minutes").toInt();
            s.lateWindowMinutes = query.value("late_window_minutes").toInt();
            s.sessionCode = query.value("session_code").toString();
            s.codeGeneratedAt = query.value("code_generated_at").toDateTime();
            s.status = (query.value("status").toString() == "OPEN") ? SessionStatus::OPEN : SessionStatus::CLOSED;
            s.closedAt = query.value("closed_at").toDateTime();
            list.push_back(s);
        }
    }
    return list;
}

int SessionRepository::createSession(const AttendanceSession& session) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO attendance_sessions (course_id, user_id, start_time, duration_minutes, "
                  "present_window_minutes, late_window_minutes, session_code, code_generated_at, status) "
                  "VALUES (:cid, :uid, NOW(), :dur, :pres, :late, :code, NOW(), 'OPEN')");
    query.bindValue(":cid", session.courseId);
    query.bindValue(":uid", session.lecturerUserId);
    query.bindValue(":dur", session.durationMinutes);
    query.bindValue(":pres", session.presentWindowMinutes);
    query.bindValue(":late", session.lateWindowMinutes);
    query.bindValue(":code", session.sessionCode);

    if (query.exec()) {
        return query.lastInsertId().toInt();
    }
    Logger::logError("SessionRepository::createSession failed: " + query.lastError().text());
    return -1;
}

bool SessionRepository::updateSessionCode(int sessionId, const QString& newCode) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE attendance_sessions SET session_code = :code, code_generated_at = NOW() WHERE session_id = :id");
    query.bindValue(":code", newCode);
    query.bindValue(":id", sessionId);
    return query.exec();
}

bool SessionRepository::closeSession(int sessionId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE attendance_sessions SET status = 'CLOSED', closed_at = NOW() WHERE session_id = :id");
    query.bindValue(":id", sessionId);
    return query.exec();
}
