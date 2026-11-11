#ifndef SESSIONREPOSITORY_H
#define SESSIONREPOSITORY_H

#include "../models/AttendanceSession.h"
#include <optional>
#include <vector>

class SessionRepository {
public:
    std::optional<AttendanceSession> findById(int sessionId);
    std::optional<AttendanceSession> findOpenSessionForCourse(int courseId);
    std::vector<AttendanceSession> findByCourse(int courseId);

    int createSession(const AttendanceSession& session);
    bool updateSessionCode(int sessionId, const QString& newCode);
    bool closeSession(int sessionId);
};

#endif // SESSIONREPOSITORY_H
