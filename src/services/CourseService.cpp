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
                     QStringLiteral("Lecturer not found."),
                     QStringLiteral("Assigned user must have LECTURER role."));
}

ServiceResult checkStudent(QSqlDatabase &db, int studentId)
{
    return checkRole(db, studentId, QStringLiteral("STUDENT"),
                     QStringLiteral("Student not found."),
                     QStringLiteral("Can only enroll users with STUDENT role."));
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
        return ServiceResult::failure(QStringLiteral("Course code and name cannot be empty."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    const ServiceResult lecturer = checkLecturer(db, lecturerId);
    if (!lecturer.ok)
        return lecturer;

    if (courseCodeTaken(db, c, 0))
        return ServiceResult::failure(QStringLiteral("Course code already exists."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("INSERT INTO courses (code, name, user_id) VALUES (?, ?, ?)"));
    q.addBindValue(c);
    q.addBindValue(n);
    q.addBindValue(lecturerId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Course code already exists."));
        qWarning() << "createCourse failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to create course. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Course created successfully."), lastInsertId(db));
}

ServiceResult CourseService::updateCourse(int courseId, const QString &code,
                                          const QString &name)
{
    const QString c = code.trimmed();
    const QString n = name.trimmed();
    if (c.isEmpty() || n.isEmpty())
        return ServiceResult::failure(QStringLiteral("Course code and name cannot be empty."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Course not found."));
    if (courseCodeTaken(db, c, courseId))
        return ServiceResult::failure(QStringLiteral("Course code already exists."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE courses SET code = ?, name = ? WHERE course_id = ?"));
    q.addBindValue(c);
    q.addBindValue(n);
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Course code already exists."));
        qWarning() << "updateCourse failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to update course. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Course updated successfully."), courseId);
}

ServiceResult CourseService::assignLecturer(int courseId, int lecturerId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Course not found."));

    const ServiceResult lecturer = checkLecturer(db, lecturerId);
    if (!lecturer.ok)
        return lecturer;

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE courses SET user_id = ? WHERE course_id = ?"));
    q.addBindValue(lecturerId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        qWarning() << "assignLecturer failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to assign lecturer. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Lecturer assigned successfully."), courseId);
}

ServiceResult CourseService::deleteCourse(int courseId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Course not found."));

    const QString blocked = QStringLiteral(
        "Cannot delete a course that already has attendance sessions recorded.");

    QSqlQuery count(db);
    count.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM attendance_sessions WHERE course_id = ?"));
    count.addBindValue(courseId);
    if (!count.exec() || !count.next())
        return ServiceResult::failure(QStringLiteral("Failed to check attendance sessions. Please try again."));
    if (count.value(0).toInt() > 0)
        return ServiceResult::failure(blocked);

    QSqlQuery q(db);
    q.prepare(QStringLiteral("DELETE FROM courses WHERE course_id = ?"));
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isForeignKeyError(q.lastError()))
            return ServiceResult::failure(blocked);
        qWarning() << "deleteCourse failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to delete course. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Course deleted successfully."), courseId);
}

ServiceResult CourseService::enrollStudent(int courseId, int studentId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Course not found."));

    const ServiceResult student = checkStudent(db, studentId);
    if (!student.ok)
        return student;

    QSqlQuery dup(db);
    dup.prepare(QStringLiteral(
        "SELECT 1 FROM enrollments WHERE user_id = ? AND course_id = ?"));
    dup.addBindValue(studentId);
    dup.addBindValue(courseId);
    if (dup.exec() && dup.next())
        return ServiceResult::failure(QStringLiteral("Student is already enrolled in this course."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "INSERT INTO enrollments (user_id, course_id, enrolled_at) VALUES (?, ?, CURDATE())"));
    q.addBindValue(studentId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Student is already enrolled in this course."));
        qWarning() << "enrollStudent failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to enroll student. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Student enrolled successfully."), lastInsertId(db));
}

ServiceResult CourseService::unenrollStudent(int courseId, int studentId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Cannot connect to database."));

    QSqlQuery find(db);
    find.prepare(QStringLiteral(
        "SELECT 1 FROM enrollments WHERE user_id = ? AND course_id = ?"));
    find.addBindValue(studentId);
    find.addBindValue(courseId);
    if (!find.exec() || !find.next())
        return ServiceResult::failure(QStringLiteral("Student is not enrolled in this course."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "DELETE FROM enrollments WHERE user_id = ? AND course_id = ?"));
    q.addBindValue(studentId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        qWarning() << "unenrollStudent failed:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Failed to unenroll student. Please try again."));
    }
    return ServiceResult::success(QStringLiteral("Student unenrolled successfully."));
}
