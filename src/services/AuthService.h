#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../models/User.h"
#include "../repositories/UserRepository.h"
#include <optional>
#include <QString>

class AuthService {
public:
    static AuthService& instance();

    bool login(const QString& email, const QString& password);

    void logout();

    bool isLoggedIn() const;
    std::optional<User> currentUser() const;

    QString lastError() const;

    static QString hashPassword(const QString& password);

    static bool verifyPassword(const QString& password, const QString& storedHash);

private:
    AuthService() = default;
    UserRepository m_userRepo;
    std::optional<User> m_currentUser;
    QString m_lastError;
};

#endif
