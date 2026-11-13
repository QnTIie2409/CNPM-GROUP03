#include "SessionService.h"
#include <QRandomGenerator>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "ServiceCommon.h"

SessionService::SessionService() {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SessionService::onRotateCode);
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
        return ServiceResult::failure("Thời gian các khung giờ không hợp lệ: phải có 0 < Đúng giờ <= Đi muộn <= Tổng thời gian.");
    }

    QSqlDatabase db = serviceDb();
    if (!db.isOpen()) {
        return ServiceResult::failure("Không thể kết nối tới cơ sở dữ liệu.");
    }

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT session_id FROM attendance_sessions WHERE course_id = ? AND status = 'OPEN' LIMIT 1");
    checkQuery.addBindValue(courseId);
    if (checkQuery.exec() && checkQuery.next()) {
        return ServiceResult::failure("Môn học này hiện đang có một phiên điểm danh đang mở!");
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
        qWarning() << "openSession thất bại:" << insertQuery.lastError().text();
        return ServiceResult::failure("Không thể tạo phiên điểm danh mới trên database.");
    }

    qint64 newSessionId = 0;
    QSqlQuery idQuery(db);
    if (idQuery.exec("SELECT LAST_INSERT_ID()") && idQuery.next()) {
        newSessionId = idQuery.value(0).toLongLong();
    }

    m_activeSessionId = newSessionId;
    m_timer->start(60000); 

    return ServiceResult::success("Mở phiên điểm danh thành công! Mã đầu tiên: " + code, newSessionId);
}


ServiceResult SessionService::closeSession(int sessionId) 
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen()) {
        return ServiceResult::failure("Không thể kết nối tới cơ sở dữ liệu.");
    }

    auto sOpt = m_sessionRepo.findById(sessionId);
    if (!sOpt.has_value()) {
        return ServiceResult::failure("Không tìm thấy phiên điểm danh.");
    }

    if (sOpt->status == SessionStatus::CLOSED) {
        return ServiceResult::failure("Phiên điểm danh này đã được đóng trước đó rồi.");
    }

    db.transaction();

    QSqlQuery closeQuery(db);
    closeQuery.prepare("UPDATE attendance_sessions SET status = 'CLOSED', closed_at = NOW() WHERE session_id = ?");
    closeQuery.addBindValue(sessionId);
    if (!closeQuery.exec()) {
        db.rollback();
        qWarning() << "closeSession thất bại:" << closeQuery.lastError().text();
        return ServiceResult::failure("Không thể cập nhật trạng thái đóng phiên.");
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
        qWarning() << "Đánh vắng tự động thất bại:" << absentQuery.lastError().text();
        return ServiceResult::failure("Lỗi khi tự động đánh vắng sinh viên chưa điểm danh.");
    }

    db.commit();

    if (m_activeSessionId == sessionId) {
        m_timer->stop();
        m_activeSessionId = -1;
    }

    emit sessionClosed(sessionId);

    QString msg = QString("Đã đóng phiên điểm danh thành công! Đã tự động ghi nhận %1 sinh viên vắng mặt (Absent).")
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
    if (m_activeSessionId > 0) {
        QString newCode = generateRandomCode();
        m_sessionRepo.updateSessionCode(m_activeSessionId, newCode);
        emit codeRotated(m_activeSessionId, newCode);
    }
}

