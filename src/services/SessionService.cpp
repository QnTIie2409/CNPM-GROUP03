#include "SessionService.h"
#include <QRandomGenerator>

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

int SessionService::openSession(int courseId, int lecturerUserId, int durationMin, int presentMin, int lateMin) {
    AttendanceSession s;
    s.courseId = courseId;
    s.lecturerUserId = lecturerUserId;
    s.durationMinutes = durationMin;
    s.presentWindowMinutes = presentMin;
    s.lateWindowMinutes = lateMin;
    s.sessionCode = generateRandomCode();

    int newId = m_sessionRepo.createSession(s);
    if (newId > 0) {
        m_activeSessionId = newId;
        m_timer->start(60000); // 60 giây xoay mã 1 lần
    }
    return newId;
}

bool SessionService::closeSession(int sessionId) {
    auto sOpt = m_sessionRepo.findById(sessionId);
    if (!sOpt.has_value()) return false;

    // 1. Đóng phiên
    bool ok = m_sessionRepo.closeSession(sessionId);
    if (ok) {
        // 2. Tự động đánh vắng tất cả sinh viên chưa check-in (FR-06)
        m_attendanceRepo.markAbsentForUnchecked(sessionId, sOpt->courseId);
        if (m_activeSessionId == sessionId) {
            m_timer->stop();
            m_activeSessionId = -1;
        }
        emit sessionClosed(sessionId);
    }
    return ok;
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
