#include "AuthService.h"

#include <QCryptographicHash>
#include <QPasswordDigestor>   // thuộc module Qt Network
#include <QRandomGenerator>
#include <QStringList>

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

// So sánh thời gian không phụ thuộc vị trí byte khác nhau.
bool constantTimeEqual(const QByteArray &a, const QByteArray &b)
{
    if (a.size() != b.size())
        return false;
    int diff = 0;
    for (int i = 0; i < a.size(); ++i)
        diff |= (a.at(i) ^ b.at(i));
    return diff == 0;
}

// Hash MD5 hex của bản demo trước đây.
QString legacyMd5(const QString &password)
{
    return QString::fromLatin1(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex());
}

} // namespace

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

    // Hash cũ (MD5 hex) của bản demo.
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

    // NFR-03: sai email hay sai mật khẩu đều dùng chung một thông báo,
    // không để lộ email có tồn tại hay không.
    const QString genericError = QStringLiteral("Email hoặc mật khẩu không đúng.");

    if (email.trimmed().isEmpty() || password.isEmpty()) {
        m_lastError = genericError;
        return false;
    }

    auto userOpt = m_userRepo.findByEmail(email.trimmed());
    if (!userOpt.has_value()) {
        // Email không tồn tại: vẫn băm một lần để thời gian phản hồi gần giống
        // trường hợp email đúng.
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

    // is_active == true mới được đăng nhập (chỉ báo sau khi mật khẩu đã đúng).
    if (!u.isActive) {
        m_lastError = QStringLiteral(
            "Tài khoản đã bị vô hiệu hóa. Vui lòng liên hệ quản trị viên.");
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
