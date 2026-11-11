#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

enum class UserRole {
    STUDENT,
    LECTURER,
    ADMIN
};

struct User {
    int userId = 0;
    UserRole role = UserRole::STUDENT;
    QString code;
    QString fullName;
    QString email;
    QString passwordHash;
    bool isActive = true;
    QDateTime createdAt;

    static QString roleToString(UserRole r) {
        switch (r) {
            case UserRole::STUDENT: return "STUDENT";
            case UserRole::LECTURER: return "LECTURER";
            case UserRole::ADMIN: return "ADMIN";
        }
        return "STUDENT";
    }

    static UserRole stringToRole(const QString& s) {
        if (s == "ADMIN") return UserRole::ADMIN;
        if (s == "LECTURER") return UserRole::LECTURER;
        return UserRole::STUDENT;
    }
};

#endif // USER_H
