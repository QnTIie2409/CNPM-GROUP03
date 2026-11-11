#ifndef SESSIONSERVICE_H
#define SESSIONSERVICE_H

#include "../models/AttendanceSession.h"
#include "../repositories/SessionRepository.h"
#include "../repositories/AttendanceRepository.h"
#include <QObject>
#include <QTimer>
#include <optional>

class SessionService : public QObject {
    Q_OBJECT
public:
    static SessionService& instance();

    int openSession(int courseId, int lecturerUserId, int durationMin = 80, int presentMin = 20, int lateMin = 80);
    bool closeSession(int sessionId);
    std::optional<AttendanceSession> getOpenSession(int courseId);
    std::optional<AttendanceSession> getSessionById(int sessionId);

    static QString generateRandomCode();

signals:
    void codeRotated(int sessionId, const QString& newCode);
    void sessionClosed(int sessionId);

private slots:
    void onRotateCode();

private:
    SessionService();
    SessionRepository m_sessionRepo;
    AttendanceRepository m_attendanceRepo;
    QTimer* m_timer = nullptr;
    int m_activeSessionId = -1;
};

#endif // SESSIONSERVICE_H
