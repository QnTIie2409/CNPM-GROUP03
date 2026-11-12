#include "AccountService.h"

#include <QDebug>
#include <QSqlQuery>
#include <QVariant>

#include "AuthService.h"

namespace {

// column chỉ nhận hằng số nội bộ ("email" hoặc "code"), không bao giờ nhận
// dữ liệu người dùng nhập vào.
bool valueTaken(QSqlDatabase &db, const QString &column, const QString &value)
{
    QSqlQuery q(db);
    q.prepare(QStringLiteral("SELECT 1 FROM users WHERE %1 = ? LIMIT 1").arg(column));
    q.addBindValue(value);
    return q.exec() && q.next();
}

qint64 lastInsertId(QSqlDatabase &db)
{
    // QODBC không hỗ trợ QSqlQuery::lastInsertId() ổn định, nên hỏi MySQL trực tiếp
    // (cùng một kết nối nên giá trị đúng với lệnh INSERT vừa chạy).
    QSqlQuery q(db);
    if (q.exec(QStringLiteral("SELECT LAST_INSERT_ID()")) && q.next())
        return q.value(0).toLongLong();
    return 0;
}

} // namespace

ServiceResult AccountService::createAccount(const QString &role, const QString &code,
                                            const QString &fullName, const QString &email,
                                            const QString &password)
{
    const QString r = role.trimmed().toUpper();
    if (r != QLatin1String("STUDENT") && r != QLatin1String("LECTURER"))
        return ServiceResult::failure(QStringLiteral("Vai trò phải là sinh viên hoặc giảng viên."));

    const QString c = code.trimmed();
    const QString name = fullName.trimmed();
    const QString mail = email.trimmed().toLower();

    if (c.isEmpty() || name.isEmpty() || mail.isEmpty() || password.isEmpty())
        return ServiceResult::failure(QStringLiteral("Vui lòng nhập đầy đủ mã, họ tên, email và mật khẩu."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    // Mã và email phải duy nhất.
    if (valueTaken(db, QStringLiteral("code"), c))
        return ServiceResult::failure(QStringLiteral("Mã này đã tồn tại."));
    if (valueTaken(db, QStringLiteral("email"), mail))
        return ServiceResult::failure(QStringLiteral("Email đã được sử dụng."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "INSERT INTO users (role, code, full_name, email, password_hash, is_active) "
        "VALUES (?, ?, ?, ?, ?, TRUE)"));
    q.addBindValue(r);
    q.addBindValue(c);
    q.addBindValue(name);
    q.addBindValue(mail);
    q.addBindValue(AuthService::hashPassword(password));

    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Mã hoặc email đã tồn tại."));
        qWarning() << "createAccount thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể tạo tài khoản. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Tạo tài khoản thành công."), lastInsertId(db));
}

ServiceResult AccountService::deactivate(int userId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    QSqlQuery check(db);
    check.prepare(QStringLiteral("SELECT 1 FROM users WHERE user_id = ?"));
    check.addBindValue(userId);
    if (!check.exec() || !check.next())
        return ServiceResult::failure(QStringLiteral("Không tìm thấy tài khoản."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE users SET is_active = 0 WHERE user_id = ?"));
    q.addBindValue(userId);
    if (!q.exec()) {
        qWarning() << "deactivate thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể vô hiệu hóa tài khoản. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Đã vô hiệu hóa tài khoản."));
}
