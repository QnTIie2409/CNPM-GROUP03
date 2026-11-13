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
        return ServiceResult::failure("Thá»i gian cĂ¡c khung giá» khĂ´ng há»£p lá»‡: pháº£i cĂ³ 0 < ÄĂºng giá» <= Äi muá»™n <= Tá»•ng thá»i gian.");
    }

    QSqlDatabase db = serviceDb();
    if (!db.isOpen()) {
        return ServiceResult::failure("KhĂ´ng thá»ƒ káº¿t ná»‘i tá»›i cÆ¡ sá»Ÿ dá»¯ liá»‡u.");
    }

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT session_id FROM attendance_sessions WHERE course_id = ? AND status = 'OPEN' LIMIT 1");
    checkQuery.addBindValue(courseId);
    if (checkQuery.exec() && checkQuery.next()) {
        return ServiceResult::failure("MĂ´n há»c nĂ y hiá»‡n Ä‘ang cĂ³ má»™t phiĂªn Ä‘iá»ƒm danh Ä‘ang má»Ÿ!");
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
        qWarning() << "openSession tháº¥t báº¡i:" << insertQuery.lastError().text();
        return ServiceResult::failure("KhĂ´ng thá»ƒ táº¡o phiĂªn Ä‘iá»ƒm danh má»›i trĂªn database.");
    }

    qint64 newSessionId = 0;
    QSqlQuery idQuery(db);
    if (idQuery.exec("SELECT LAST_INSERT_ID()") && idQuery.next()) {
        newSessionId = idQuery.value(0).toLongLong();
    }

    m_activeSessionId = newSessionId;
    m_timer->start(60000); 

    return ServiceResult::success("Má»Ÿ phiĂªn Ä‘iá»ƒm danh thĂ nh cĂ´ng! MĂ£ Ä‘áº§u tiĂªn: " + code, newSessionId);
}

ServiceResult SessionService::closeSession(int sessionId) 
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen()) {
        return ServiceResult::failure("KhĂ´ng thá»ƒ káº¿t ná»‘i tá»›i cÆ¡ sá»Ÿ dá»¯ liá»‡u.");
    }

    auto sOpt = m_sessionRepo.findById(sessionId);
    if (!sOpt.has_value()) {
        return ServiceResult::failure("KhĂ´ng tĂ¬m tháº¥y phiĂªn Ä‘iá»ƒm danh.");
    }

    if (sOpt->status == SessionStatus::CLOSED) {
        return ServiceResult::failure("PhiĂªn Ä‘iá»ƒm danh nĂ y Ä‘Ă£ Ä‘Æ°á»£c Ä‘Ă³ng trÆ°á»›c Ä‘Ă³ rá»“i.");
    }

    db.transaction();

    QSqlQuery closeQuery(db);
    closeQuery.prepare("UPDATE attendance_sessions SET status = 'CLOSED', closed_at = NOW() WHERE session_id = ?");
    closeQuery.addBindValue(sessionId);
    if (!closeQuery.exec()) {
        db.rollback();
        qWarning() << "closeSession tháº¥t báº¡i:" << closeQuery.lastError().text();
        return ServiceResult::failure("KhĂ´ng thá»ƒ cáº­p nháº­t tráº¡ng thĂ¡i Ä‘Ă³ng phiĂªn.");
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
        qWarning() << "ÄĂ¡nh váº¯ng tá»± Ä‘á»™ng tháº¥t báº¡i:" << absentQuery.lastError().text();
        return ServiceResult::failure("Lá»—i khi tá»± Ä‘á»™ng Ä‘Ă¡nh váº¯ng sinh viĂªn chÆ°a Ä‘iá»ƒm danh.");
    }

    db.commit();

    if (m_activeSessionId == sessionId) {
        m_timer->stop();
        m_activeSessionId = -1;
    }

    emit sessionClosed(sessionId);

    QString msg = QString("ÄĂ£ Ä‘Ă³ng phiĂªn Ä‘iá»ƒm danh thĂ nh cĂ´ng! ÄĂ£ tá»± Ä‘á»™ng ghi nháº­n %1 sinh viĂªn váº¯ng máº·t (Absent).")
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

