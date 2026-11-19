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

std::vector<AttendanceSession> SessionRepository::findOpenSessions() {
    std::vector<AttendanceSession> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT session_id, course_id, user_id, start_time, duration_minutes, "
                  "present_window_minutes, late_window_minutes, session_code, code_generated_at, status, closed_at "
                  "FROM attendance_sessions WHERE status = 'OPEN' ORDER BY start_time");
    if (!query.exec()) {
        Logger::logError("SessionRepository::findOpenSessions failed: " + query.lastError().text());
        return list;
    }
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
        s.status = SessionStatus::OPEN;
        s.closedAt = query.value("closed_at").toDateTime();
        list.push_back(s);
    }
    return list;
}

bool SessionRepository::lecturerTeachesCourse(int lecturerUserId, int courseId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT 1 FROM courses c JOIN users u ON u.user_id = c.user_id "
                  "WHERE c.course_id = :course_id AND c.user_id = :lecturer_id "
                  "AND u.role = 'LECTURER' AND u.is_active = TRUE LIMIT 1");
    query.bindValue(":course_id", courseId);
    query.bindValue(":lecturer_id", lecturerUserId);
    if (!query.exec()) {
        Logger::logError("SessionRepository::lecturerTeachesCourse failed: " + query.lastError().text());
        return false;
    }
    return query.next();
}

qint64 SessionRepository::serverSecondsSince(const QDateTime &startTime) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT TIMESTAMPDIFF(SECOND, :start_time, NOW())");
    // DATETIME is a wall-clock value. Avoid Qt attaching/converting time zones
    // when binding a QDateTime returned by the SQL driver.
    query.bindValue(":start_time", startTime.toString("yyyy-MM-dd HH:mm:ss"));
    if (!query.exec() || !query.next()) {
        Logger::logError("SessionRepository::serverSecondsSince failed: " + query.lastError().text());
        return -1;
    }
    return query.value(0).toLongLong();
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
    // Only one client may rotate this row per interval. A second client's
    // update affects zero rows after the first client has refreshed the time.
    query.prepare("UPDATE attendance_sessions SET session_code = :code, code_generated_at = NOW() "
                  "WHERE session_id = :id AND status = 'OPEN' "
                  "AND code_generated_at <= DATE_SUB(NOW(), INTERVAL 60 SECOND) "
                  "AND NOW() < TIMESTAMPADD(MINUTE, duration_minutes, start_time)");
    query.bindValue(":code", newCode);
    query.bindValue(":id", sessionId);
    if (!query.exec()) {
        Logger::logError("SessionRepository::updateSessionCode failed: " + query.lastError().text());
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool SessionRepository::closeSession(int sessionId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE attendance_sessions SET status = 'CLOSED', closed_at = NOW() WHERE session_id = :id");
    query.bindValue(":id", sessionId);
    return query.exec();
}
