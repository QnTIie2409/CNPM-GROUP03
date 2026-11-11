#include "AuthService.h"
#include <QCryptographicHash>

AuthService& AuthService::instance() {
    static AuthService inst;
    return inst;
}

QString AuthService::hashPassword(const QString& password) {
    // Basic hash using MD5/SHA256 for student system demo
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
}

bool AuthService::login(const QString& email, const QString& password) {
    auto userOpt = m_userRepo.findByEmail(email.trimmed());
    if (!userOpt.has_value()) {
        return false;
    }

    const User& u = userOpt.value();
    if (!u.isActive) {
        return false;
    }

    QString inputHash = hashPassword(password);
    if (u.passwordHash == inputHash || u.passwordHash == password) {
        m_currentUser = u;
        return true;
    }

    return false;
}

void AuthService::logout() {
    m_currentUser = std::nullopt;
}

bool AuthService::isLoggedIn() const {
    return m_currentUser.has_value();
}

std::optional<User> AuthService::currentUser() const {
    return m_currentUser;
}
