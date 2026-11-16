#ifndef SESSIONREPOSITORY_H
#define SESSIONREPOSITORY_H

#include "../models/AttendanceSession.h"
#include <optional>
#include <vector>

class SessionRepository {
public:
    std::optional<AttendanceSession> findById(int sessionId);
    std::optional<AttendanceSession> findOpenSessionForCourse(int courseId);
    std::vector<AttendanceSession> findOpenSessions();
    std::vector<AttendanceSession> findByCourse(int courseId);

    bool lecturerTeachesCourse(int lecturerUserId, int courseId);
    qint64 serverSecondsSince(const QDateTime &startTime);

    int createSession(const AttendanceSession& session);
    bool updateSessionCode(int sessionId, const QString& newCode);
    bool closeSession(int sessionId);
};

#endif
