#include "AttendanceRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>

std::optional<AttendanceRecord> AttendanceRepository::findRecord(int sessionId, int studentUserId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT record_id, session_id, user_id, status, check_in_time "
                  "FROM attendance_records WHERE session_id = :sid AND user_id = :uid");
    query.bindValue(":sid", sessionId);
    query.bindValue(":uid", studentUserId);

    if (query.exec() && query.next()) {
        AttendanceRecord r;
        r.recordId = query.value("record_id").toInt();
        r.sessionId = query.value("session_id").toInt();
        r.studentUserId = query.value("user_id").toInt();
        r.status = AttendanceRecord::stringToStatus(query.value("status").toString());
        r.checkInTime = query.value("check_in_time").toDateTime();
        return r;
    }
    return std::nullopt;
}

std::vector<AttendanceRecord> AttendanceRepository::findBySession(int sessionId) {
    std::vector<AttendanceRecord> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT r.record_id, r.session_id, r.user_id, r.status, r.check_in_time, "
                  "u.code as student_code, u.full_name as student_name "
                  "FROM attendance_records r "
                  "JOIN users u ON r.user_id = u.user_id "
                  "WHERE r.session_id = :sid");
    query.bindValue(":sid", sessionId);
    if (query.exec()) {
        while (query.next()) {
            AttendanceRecord r;
            r.recordId = query.value("record_id").toInt();
            r.sessionId = query.value("session_id").toInt();
            r.studentUserId = query.value("user_id").toInt();
            r.studentCode = query.value("student_code").toString();
            r.studentName = query.value("student_name").toString();
            r.status = AttendanceRecord::stringToStatus(query.value("status").toString());
            r.checkInTime = query.value("check_in_time").toDateTime();
            list.push_back(r);
        }
    }
    return list;
}

std::vector<AttendanceRecord> AttendanceRepository::findByStudentAndCourse(int studentUserId, int courseId) {
    std::vector<AttendanceRecord> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT r.record_id, r.session_id, r.user_id, r.status, r.check_in_time "
                  "FROM attendance_records r "
                  "JOIN attendance_sessions s ON r.session_id = s.session_id "
                  "WHERE r.user_id = :uid AND s.course_id = :cid ORDER BY s.start_time DESC");
    query.bindValue(":uid", studentUserId);
    query.bindValue(":cid", courseId);
    if (query.exec()) {
        while (query.next()) {
            AttendanceRecord r;
            r.recordId = query.value("record_id").toInt();
            r.sessionId = query.value("session_id").toInt();
            r.studentUserId = query.value("user_id").toInt();
            r.status = AttendanceRecord::stringToStatus(query.value("status").toString());
            r.checkInTime = query.value("check_in_time").toDateTime();
            list.push_back(r);
        }
    }
    return list;
}

bool AttendanceRepository::saveCheckIn(int sessionId, int studentUserId, AttendanceStatus status) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO attendance_records (session_id, user_id, status, check_in_time) "
                  "VALUES (:sid, :uid, :status, NOW())");
    query.bindValue(":sid", sessionId);
    query.bindValue(":uid", studentUserId);
    query.bindValue(":status", AttendanceRecord::statusToString(status));

    if (!query.exec()) {
        Logger::logError("AttendanceRepository::saveCheckIn failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool AttendanceRepository::markAbsentForUnchecked(int sessionId, int courseId) {
    // Tìm các sinh viên trong enrollments mà chưa có record trong attendance_records của session này
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO attendance_records (session_id, user_id, status, check_in_time) "
                  "SELECT :sid, e.user_id, 'ABSENT', NULL "
                  "FROM enrollments e "
                  "WHERE e.course_id = :cid "
                  "AND e.user_id NOT IN (SELECT user_id FROM attendance_records WHERE session_id = :sid2)");
    query.bindValue(":sid", sessionId);
    query.bindValue(":cid", courseId);
    query.bindValue(":sid2", sessionId);

    if (!query.exec()) {
        Logger::logError("AttendanceRepository::markAbsentForUnchecked failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool AttendanceRepository::updateStatusWithLog(int recordId, AttendanceStatus newStatus, int lecturerUserId, const QString& reason) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    db.transaction();

    // 1. Lấy old status
    QSqlQuery qOld(db);
    qOld.prepare("SELECT status FROM attendance_records WHERE record_id = :rid");
    qOld.bindValue(":rid", recordId);
    if (!qOld.exec() || !qOld.next()) {
        db.rollback();
        return false;
    }
    QString oldStatusStr = qOld.value("status").toString();
    QString newStatusStr = AttendanceRecord::statusToString(newStatus);

    // 2. Cập nhật record
    QSqlQuery qUp(db);
    qUp.prepare("UPDATE attendance_records SET status = :status WHERE record_id = :rid");
    qUp.bindValue(":status", newStatusStr);
    qUp.bindValue(":rid", recordId);
    if (!qUp.exec()) {
        db.rollback();
        return false;
    }

    // 3. Ghi vào change log
    QSqlQuery qLog(db);
    qLog.prepare("INSERT INTO attendance_change_logs (record_id, old_status, new_status, reason, changed_at, user_id) "
                 "VALUES (:rid, :old, :new, :reason, NOW(), :uid)");
    qLog.bindValue(":rid", recordId);
    qLog.bindValue(":old", oldStatusStr);
    qLog.bindValue(":new", newStatusStr);
    qLog.bindValue(":reason", reason);
    qLog.bindValue(":uid", lecturerUserId);
    if (!qLog.exec()) {
        db.rollback();
        return false;
    }

    return db.commit();
}
