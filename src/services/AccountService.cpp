#include "AccountService.h"

#include <QDebug>
#include <QSqlQuery>
#include <QVariant>

#include "AuthService.h"

namespace {

bool valueTaken(QSqlDatabase &db, const QString &column, const QString &value)
{
    QSqlQuery q(db);
    q.prepare(QStringLiteral("SELECT 1 FROM users WHERE %1 = ? LIMIT 1").arg(column));
    q.addBindValue(value);
    return q.exec() && q.next();
}

qint64 lastInsertId(QSqlDatabase &db)
{
    QSqlQuery q(db);
    if (q.exec(QStringLiteral("SELECT LAST_INSERT_ID()")) && q.next())
        return q.value(0).toLongLong();
    return 0;
}

}

ServiceResult AccountService::createAccount(const QString &role, const QString &code,
                                            const QString &fullName, const QString &email,
                                            const QString &password)
{
    const QString r = role.trimmed().toUpper();
    if (r != QLatin1String("STUDENT") && r != QLatin1String("LECTURER"))
        return ServiceResult::failure(QStringLiteral("Role must be STUDENT or LECTURER."));

    const QString c = code.trimmed();
    const QString name = fullName.trimmed();
    const QString mail = email.trimmed().toLower();

    if (c.isEmpty() || name.isEmpty() || mail.isEmpty() || password.isEmpty())
        return ServiceResult::failure(QStringLiteral("Please provide complete code, full name, email, and password."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    if (valueTaken(db, QStringLiteral("code"), c))
        return ServiceResult::failure(QStringLiteral("This user code is already in use."));
    if (valueTaken(db, QStringLiteral("email"), mail))
        return ServiceResult::failure(QStringLiteral("This email address is already in use."));

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
            return ServiceResult::failure(QStringLiteral("Code or email already exists."));
        qWarning() << "createAccount failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to create account. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Account created successfully."), lastInsertId(db));
}

ServiceResult AccountService::deactivate(int userId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    QSqlQuery check(db);
    check.prepare(QStringLiteral("SELECT 1 FROM users WHERE user_id = ?"));
    check.addBindValue(userId);
    if (!check.exec() || !check.next())
        return ServiceResult::failure(QStringLiteral("Account not found."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE users SET is_active = 0 WHERE user_id = ?"));
    q.addBindValue(userId);
    if (!q.exec()) {
        qWarning() << "deactivate failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to deactivate account. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Account deactivated successfully."));
}
