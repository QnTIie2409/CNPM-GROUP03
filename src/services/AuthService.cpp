#include "AuthService.h"

#include <QCryptographicHash>
#include <QPasswordDigestor>
#include <QRandomGenerator>
#include <QStringList>
#include "../repositories/DatabaseManager.h"

namespace {

constexpr int kIterations = 100000;
constexpr int kSaltBytes = 16;
constexpr int kKeyBytes = 32;
const QLatin1String kAlgoTag("pbkdf2_sha256");

QByteArray deriveKey(const QString &password, const QByteArray &salt,
                     int iterations, int keyBytes)
{
    return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                              password.toUtf8(), salt,
                                              iterations, keyBytes);
}

bool constantTimeEqual(const QByteArray &a, const QByteArray &b)
{
    if (a.size() != b.size())
        return false;
    int diff = 0;
    for (int i = 0; i < a.size(); ++i)
        diff |= (a.at(i) ^ b.at(i));
    return diff == 0;
}

QString legacyMd5(const QString &password)
{
    return QString::fromLatin1(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
}

}

AuthService& AuthService::instance() {
    static AuthService inst;
    return inst;
}

QString AuthService::hashPassword(const QString& password) {
    QByteArray salt(kSaltBytes, '\0');
    for (int i = 0; i < kSaltBytes; ++i)
        salt[i] = static_cast<char>(QRandomGenerator::system()->bounded(256));

    const QByteArray key = deriveKey(password, salt, kIterations, kKeyBytes);

    return QStringLiteral("%1$%2$%3$%4")
        .arg(kAlgoTag)
        .arg(kIterations)
        .arg(QString::fromLatin1(salt.toHex()))
        .arg(QString::fromLatin1(key.toHex()));
}

bool AuthService::verifyPassword(const QString& password, const QString& storedHash) {
    if (storedHash.isEmpty())
        return false;

    if (!storedHash.startsWith(kAlgoTag))
        return storedHash.compare(legacyMd5(password), Qt::CaseInsensitive) == 0;

    const QStringList parts = storedHash.split(QLatin1Char('$'));
    if (parts.size() != 4)
        return false;

    bool ok = false;
    const int iterations = parts.at(1).toInt(&ok);
    if (!ok || iterations <= 0)
        return false;

    const QByteArray salt = QByteArray::fromHex(parts.at(2).toLatin1());
    const QByteArray expected = QByteArray::fromHex(parts.at(3).toLatin1());
    if (salt.isEmpty() || expected.isEmpty())
        return false;

    return constantTimeEqual(deriveKey(password, salt, iterations, expected.size()),
                             expected);
}

bool AuthService::login(const QString& email, const QString& password) {
    m_lastError.clear();

    if (!DatabaseManager::instance().isConnected()) {
        m_lastError = QStringLiteral("Cannot connect to the database. Please start MySQL and try again.");
        return false;
    }

    const QString genericError = QStringLiteral("Invalid username or password.");

    if (email.trimmed().isEmpty() || password.isEmpty()) {
        m_lastError = genericError;
        return false;
    }

    auto userOpt = m_userRepo.findByEmail(email.trimmed());
    if (!userOpt.has_value()) {
        userOpt = m_userRepo.findByCode(email.trimmed());
    }

    if (!userOpt.has_value()) {
        static const QString dummyHash = hashPassword(QStringLiteral("dummy-password"));
        (void)verifyPassword(password, dummyHash);
        m_lastError = genericError;
        return false;
    }

    const User u = userOpt.value();
    if (!verifyPassword(password, u.passwordHash)) {
        m_lastError = genericError;
        return false;
    }

    if (!u.isActive) {
        m_lastError = QStringLiteral(
            "Account has been deactivated. Please contact administrator.");
        return false;
    }

    m_currentUser = u;
    return true;
}

void AuthService::logout() {
    m_currentUser.reset();
}

bool AuthService::isLoggedIn() const {
    return m_currentUser.has_value();
}

std::optional<User> AuthService::currentUser() const {
    return m_currentUser;
}

QString AuthService::lastError() const {
    return m_lastError;
}
