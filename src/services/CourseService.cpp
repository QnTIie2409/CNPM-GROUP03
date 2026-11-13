#include "CourseService.h"

#include <QDebug>
#include <QSqlQuery>
#include <QVariant>

namespace {

bool courseExists(QSqlDatabase &db, int courseId)
{
    QSqlQuery q(db);
    q.prepare(QStringLiteral("SELECT 1 FROM courses WHERE course_id = ?"));
    q.addBindValue(courseId);
    return q.exec() && q.next();
}

bool courseCodeTaken(QSqlDatabase &db, const QString &code, int excludeCourseId)
{
    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "SELECT 1 FROM courses WHERE code = ? AND course_id <> ? LIMIT 1"));
    q.addBindValue(code);
    q.addBindValue(excludeCourseId);
    return q.exec() && q.next();
}

ServiceResult checkRole(QSqlDatabase &db, int userId, const QString &role,
                        const QString &notFoundMsg, const QString &wrongRoleMsg)
{
    QSqlQuery q(db);
    q.prepare(QStringLiteral("SELECT role FROM users WHERE user_id = ?"));
    q.addBindValue(userId);
    if (!q.exec() || !q.next())
        return ServiceResult::failure(notFoundMsg);
    if (q.value(0).toString() != role)
        return ServiceResult::failure(wrongRoleMsg);
    return ServiceResult::success();
}

ServiceResult checkLecturer(QSqlDatabase &db, int lecturerId)
{
    return checkRole(db, lecturerId, QStringLiteral("LECTURER"),
                     QStringLiteral("KhĂ´ng tĂ¬m tháº¥y giáº£ng viĂªn."),
                     QStringLiteral("NgÆ°á»i Ä‘Æ°á»£c phĂ¢n cĂ´ng pháº£i lĂ  giáº£ng viĂªn."));
}

ServiceResult checkStudent(QSqlDatabase &db, int studentId)
{
    return checkRole(db, studentId, QStringLiteral("STUDENT"),
                     QStringLiteral("KhĂ´ng tĂ¬m tháº¥y sinh viĂªn."),
                     QStringLiteral("Chá»‰ cĂ³ thá»ƒ ghi danh tĂ i khoáº£n sinh viĂªn."));
}

qint64 lastInsertId(QSqlDatabase &db)
{
    QSqlQuery q(db);
    if (q.exec(QStringLiteral("SELECT LAST_INSERT_ID()")) && q.next())
        return q.value(0).toLongLong();
    return 0;
}

}

ServiceResult CourseService::createCourse(const QString &code, const QString &name,
                                          int lecturerId)
{
    const QString c = code.trimmed();
    const QString n = name.trimmed();
    if (c.isEmpty() || n.isEmpty())
        return ServiceResult::failure(QStringLiteral("Vui lĂ²ng nháº­p mĂ£ vĂ  tĂªn mĂ´n há»c."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    const ServiceResult lecturer = checkLecturer(db, lecturerId);
    if (!lecturer.ok)
        return lecturer;

    if (courseCodeTaken(db, c, 0))
        return ServiceResult::failure(QStringLiteral("MĂ£ mĂ´n há»c Ä‘Ă£ tá»“n táº¡i."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("INSERT INTO courses (code, name, user_id) VALUES (?, ?, ?)"));
    q.addBindValue(c);
    q.addBindValue(n);
    q.addBindValue(lecturerId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("MĂ£ mĂ´n há»c Ä‘Ă£ tá»“n táº¡i."));
        qWarning() << "createCourse tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ táº¡o mĂ´n há»c. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("Táº¡o mĂ´n há»c thĂ nh cĂ´ng."), lastInsertId(db));
}

ServiceResult CourseService::updateCourse(int courseId, const QString &code,
                                          const QString &name)
{
    const QString c = code.trimmed();
    const QString n = name.trimmed();
    if (c.isEmpty() || n.isEmpty())
        return ServiceResult::failure(QStringLiteral("Vui lĂ²ng nháº­p mĂ£ vĂ  tĂªn mĂ´n há»c."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("KhĂ´ng tĂ¬m tháº¥y mĂ´n há»c."));
    if (courseCodeTaken(db, c, courseId))
        return ServiceResult::failure(QStringLiteral("MĂ£ mĂ´n há»c Ä‘Ă£ tá»“n táº¡i."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE courses SET code = ?, name = ? WHERE course_id = ?"));
    q.addBindValue(c);
    q.addBindValue(n);
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("MĂ£ mĂ´n há»c Ä‘Ă£ tá»“n táº¡i."));
        qWarning() << "updateCourse tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ cáº­p nháº­t mĂ´n há»c. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("Cáº­p nháº­t mĂ´n há»c thĂ nh cĂ´ng."), courseId);
}

ServiceResult CourseService::assignLecturer(int courseId, int lecturerId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("KhĂ´ng tĂ¬m tháº¥y mĂ´n há»c."));

    const ServiceResult lecturer = checkLecturer(db, lecturerId);
    if (!lecturer.ok)
        return lecturer;

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE courses SET user_id = ? WHERE course_id = ?"));
    q.addBindValue(lecturerId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        qWarning() << "assignLecturer tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ phĂ¢n cĂ´ng giáº£ng viĂªn. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("ÄĂ£ phĂ¢n cĂ´ng giáº£ng viĂªn."), courseId);
}

ServiceResult CourseService::deleteCourse(int courseId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("KhĂ´ng tĂ¬m tháº¥y mĂ´n há»c."));

    const QString blocked = QStringLiteral(
        "KhĂ´ng thá»ƒ xĂ³a mĂ´n há»c Ä‘Ă£ cĂ³ phiĂªn Ä‘iá»ƒm danh trong há»‡ thá»‘ng.");

    QSqlQuery count(db);
    count.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM attendance_sessions WHERE course_id = ?"));
    count.addBindValue(courseId);
    if (!count.exec() || !count.next())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng kiá»ƒm tra Ä‘Æ°á»£c dá»¯ liá»‡u Ä‘iá»ƒm danh. Vui lĂ²ng thá»­ láº¡i."));
    if (count.value(0).toInt() > 0)
        return ServiceResult::failure(blocked);

    QSqlQuery q(db);
    q.prepare(QStringLiteral("DELETE FROM courses WHERE course_id = ?"));
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isForeignKeyError(q.lastError()))
            return ServiceResult::failure(blocked);
        qWarning() << "deleteCourse tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ xĂ³a mĂ´n há»c. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("ÄĂ£ xĂ³a mĂ´n há»c."), courseId);
}

ServiceResult CourseService::enrollStudent(int courseId, int studentId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("KhĂ´ng tĂ¬m tháº¥y mĂ´n há»c."));

    const ServiceResult student = checkStudent(db, studentId);
    if (!student.ok)
        return student;

    QSqlQuery dup(db);
    dup.prepare(QStringLiteral(
        "SELECT 1 FROM enrollments WHERE user_id = ? AND course_id = ?"));
    dup.addBindValue(studentId);
    dup.addBindValue(courseId);
    if (dup.exec() && dup.next())
        return ServiceResult::failure(QStringLiteral("Sinh viĂªn Ä‘Ă£ cĂ³ trong mĂ´n há»c nĂ y."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "INSERT INTO enrollments (user_id, course_id, enrolled_at) VALUES (?, ?, CURDATE())"));
    q.addBindValue(studentId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Sinh viĂªn Ä‘Ă£ cĂ³ trong mĂ´n há»c nĂ y."));
        qWarning() << "enrollStudent tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ thĂªm sinh viĂªn vĂ o mĂ´n há»c. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("ÄĂ£ thĂªm sinh viĂªn vĂ o mĂ´n há»c."), lastInsertId(db));
}

ServiceResult CourseService::unenrollStudent(int courseId, int studentId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("KhĂ´ng káº¿t ná»‘i Ä‘Æ°á»£c cÆ¡ sá»Ÿ dá»¯ liá»‡u."));

    QSqlQuery find(db);
    find.prepare(QStringLiteral(
        "SELECT 1 FROM enrollments WHERE user_id = ? AND course_id = ?"));
    find.addBindValue(studentId);
    find.addBindValue(courseId);
    if (!find.exec() || !find.next())
        return ServiceResult::failure(QStringLiteral("Sinh viĂªn chÆ°a ghi danh vĂ o mĂ´n há»c nĂ y."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "DELETE FROM enrollments WHERE user_id = ? AND course_id = ?"));
    q.addBindValue(studentId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        qWarning() << "unenrollStudent tháº¥t báº¡i:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("KhĂ´ng thá»ƒ xĂ³a sinh viĂªn khá»i mĂ´n há»c. Vui lĂ²ng thá»­ láº¡i."));
    }
    return ServiceResult::success(QStringLiteral("ÄĂ£ xĂ³a sinh viĂªn khá»i mĂ´n há»c."));
}
