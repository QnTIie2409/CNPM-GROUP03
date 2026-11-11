#ifndef ATTENDANCESESSION_H
#define ATTENDANCESESSION_H

#include <QString>
#include <QDateTime>

enum class SessionStatus {
    OPEN,
    CLOSED
};

struct AttendanceSession {
    int sessionId = 0;
    int courseId = 0;
    int lecturerUserId = 0;
    QDateTime startTime;
    int durationMinutes = 80;
    int presentWindowMinutes = 20;
    int lateWindowMinutes = 80;
    QString sessionCode;
    QDateTime codeGeneratedAt;
    SessionStatus status = SessionStatus::OPEN;
    QDateTime closedAt;
};

#endif // ATTENDANCESESSION_H
