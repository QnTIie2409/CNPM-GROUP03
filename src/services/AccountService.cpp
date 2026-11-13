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
        return ServiceResult::failure(QStringLiteral("Vai trĂ² pháº£i lĂ  sinh viĂªn hoáº·c giáº£ng viĂªn."));

    const QString c = code.trimmed();
    const QString name = fullName.trimmed();
    const QString mail = email.trimmed().toLower();

    if (c.isEmpty() || name.isEmpty() || mail.isEmpty() || password.isEmpty())
        return ServiceResult::failure(QStringLiteral("Vui lĂ²ng nháº­p Ä‘áº§y Ä‘á»§ mĂ£, há» tĂªn, email vĂ  máº­t kháº©u."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    if (valueTaken(db, QStringLiteral("code"), c))
        return ServiceResult::failure(QStringLiteral("MĂ£ nĂ y Ä‘Ă£ tá»“n táº¡i."));
    if (valueTaken(db, QStringLiteral("email"), mail))
        return ServiceResult::failure(QStringLiteral("Email Ä‘Ă£ Ä‘Æ°á»£c sá»­ dá»¥ng."));

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
            return ServiceResult::failure(QStringLiteral("MĂ£ hoáº·c email Ä‘Ă£ tá»“n táº¡i."));
        qWarning() << "createAccount tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ táº¡o tĂ i khoáº£n. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("Táº¡o tĂ i khoáº£n thĂ nh cĂ´ng."), lastInsertId(db));
}

ServiceResult AccountService::deactivate(int userId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    QSqlQuery check(db);
    check.prepare(QStringLiteral("SELECT 1 FROM users WHERE user_id = ?"));
    check.addBindValue(userId);
    if (!check.exec() || !check.next())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng tĂ¬m tháº¥y tĂ i khoáº£n."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE users SET is_active = 0 WHERE user_id = ?"));
    q.addBindValue(userId);
    if (!q.exec()) {
        qWarning() << "deactivate tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ vĂ´ hiá»‡u hĂ³a tĂ i khoáº£n. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("ÄĂ£ vĂ´ hiá»‡u hĂ³a tĂ i khoáº£n."));
}
