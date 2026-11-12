#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../models/User.h"
#include "../repositories/UserRepository.h"
#include <optional>
#include <QString>

// FR-01, FR-02, FR-03 | UC-01, UC-02
class AuthService {
public:
    static AuthService& instance();

    // Tìm user theo email, băm mật khẩu để so khớp, kiểm tra is_active == true.
    // Thành công: lưu phiên đăng nhập, role lấy qua currentUser().
    // Sai email hay sai mật khẩu đều dùng chung một thông báo (NFR-03):
    // đọc thông báo bằng lastError().
    bool login(const QString& email, const QString& password);

    // Xóa phiên đăng nhập hiện tại.
    void logout();

    bool isLoggedIn() const;
    std::optional<User> currentUser() const;

    // Thông báo của lần login gần nhất (rỗng nếu thành công).
    QString lastError() const;

    // NFR-09: PBKDF2-HMAC-SHA256 + salt ngẫu nhiên.
    // Định dạng lưu trong users.password_hash:
    //   pbkdf2_sha256$<số vòng lặp>$<salt hex>$<hash hex>
    static QString hashPassword(const QString& password);

    // Kiểm tra mật khẩu với chuỗi đã lưu. Vẫn chấp nhận hash MD5 cũ của bản demo
    // để tài khoản có sẵn đăng nhập được.
    static bool verifyPassword(const QString& password, const QString& storedHash);

private:
    AuthService() = default;
    UserRepository m_userRepo;
    std::optional<User> m_currentUser;
    QString m_lastError;
};

#endif // AUTHSERVICE_H
