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

// Mã môn học trùng với môn khác (excludeCourseId = 0 khi tạo mới).
bool courseCodeTaken(QSqlDatabase &db, const QString &code, int excludeCourseId)
{
    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "SELECT 1 FROM courses WHERE code = ? AND course_id <> ? LIMIT 1"));
    q.addBindValue(code);
    q.addBindValue(excludeCourseId);
    return q.exec() && q.next();
}

// Kiểm tra userId tồn tại và đúng vai trò (LECTURER hoặc STUDENT).
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
                     QStringLiteral("Không tìm thấy giảng viên."),
                     QStringLiteral("Người được phân công phải là giảng viên."));
}

ServiceResult checkStudent(QSqlDatabase &db, int studentId)
{
    return checkRole(db, studentId, QStringLiteral("STUDENT"),
                     QStringLiteral("Không tìm thấy sinh viên."),
                     QStringLiteral("Chỉ có thể ghi danh tài khoản sinh viên."));
}

qint64 lastInsertId(QSqlDatabase &db)
{
    // QODBC không hỗ trợ QSqlQuery::lastInsertId() ổn định, nên hỏi MySQL trực tiếp.
    QSqlQuery q(db);
    if (q.exec(QStringLiteral("SELECT LAST_INSERT_ID()")) && q.next())
        return q.value(0).toLongLong();
    return 0;
}

} // namespace

ServiceResult CourseService::createCourse(const QString &code, const QString &name,
                                          int lecturerId)
{
    const QString c = code.trimmed();
    const QString n = name.trimmed();
    if (c.isEmpty() || n.isEmpty())
        return ServiceResult::failure(QStringLiteral("Vui lòng nhập mã và tên môn học."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    const ServiceResult lecturer = checkLecturer(db, lecturerId);
    if (!lecturer.ok)
        return lecturer;

    if (courseCodeTaken(db, c, 0))
        return ServiceResult::failure(QStringLiteral("Mã môn học đã tồn tại."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("INSERT INTO courses (code, name, user_id) VALUES (?, ?, ?)"));
    q.addBindValue(c);
    q.addBindValue(n);
    q.addBindValue(lecturerId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Mã môn học đã tồn tại."));
        qWarning() << "createCourse thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể tạo môn học. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Tạo môn học thành công."), lastInsertId(db));
}

ServiceResult CourseService::updateCourse(int courseId, const QString &code,
                                          const QString &name)
{
    const QString c = code.trimmed();
    const QString n = name.trimmed();
    if (c.isEmpty() || n.isEmpty())
        return ServiceResult::failure(QStringLiteral("Vui lòng nhập mã và tên môn học."));

    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Không tìm thấy môn học."));
    if (courseCodeTaken(db, c, courseId))
        return ServiceResult::failure(QStringLiteral("Mã môn học đã tồn tại."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE courses SET code = ?, name = ? WHERE course_id = ?"));
    q.addBindValue(c);
    q.addBindValue(n);
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Mã môn học đã tồn tại."));
        qWarning() << "updateCourse thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể cập nhật môn học. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Cập nhật môn học thành công."), courseId);
}

ServiceResult CourseService::assignLecturer(int courseId, int lecturerId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Không tìm thấy môn học."));

    const ServiceResult lecturer = checkLecturer(db, lecturerId);
    if (!lecturer.ok)
        return lecturer;

    QSqlQuery q(db);
    q.prepare(QStringLiteral("UPDATE courses SET user_id = ? WHERE course_id = ?"));
    q.addBindValue(lecturerId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        qWarning() << "assignLecturer thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể phân công giảng viên. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Đã phân công giảng viên."), courseId);
}

ServiceResult CourseService::deleteCourse(int courseId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Không tìm thấy môn học."));

    const QString blocked = QStringLiteral(
        "Không thể xóa môn học đã có phiên điểm danh trong hệ thống.");

    // FR-15: môn đã có phiên điểm danh thì cấm xóa.
    QSqlQuery count(db);
    count.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM attendance_sessions WHERE course_id = ?"));
    count.addBindValue(courseId);
    if (!count.exec() || !count.next())
        return ServiceResult::failure(QStringLiteral("Không kiểm tra được dữ liệu điểm danh. Vui lòng thử lại."));
    if (count.value(0).toInt() > 0)
        return ServiceResult::failure(blocked);

    QSqlQuery q(db);
    q.prepare(QStringLiteral("DELETE FROM courses WHERE course_id = ?"));
    q.addBindValue(courseId);
    if (!q.exec()) {
        // Khóa ngoại của attendance_sessions là lớp chặn cuối cùng.
        if (isForeignKeyError(q.lastError()))
            return ServiceResult::failure(blocked);
        qWarning() << "deleteCourse thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể xóa môn học. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Đã xóa môn học."), courseId);
}

ServiceResult CourseService::enrollStudent(int courseId, int studentId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    if (!courseExists(db, courseId))
        return ServiceResult::failure(QStringLiteral("Không tìm thấy môn học."));

    const ServiceResult student = checkStudent(db, studentId);
    if (!student.ok)
        return student;

    QSqlQuery dup(db);
    dup.prepare(QStringLiteral(
        "SELECT 1 FROM enrollments WHERE user_id = ? AND course_id = ?"));
    dup.addBindValue(studentId);
    dup.addBindValue(courseId);
    if (dup.exec() && dup.next())
        return ServiceResult::failure(QStringLiteral("Sinh viên đã có trong môn học này."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "INSERT INTO enrollments (user_id, course_id, enrolled_at) VALUES (?, ?, CURDATE())"));
    q.addBindValue(studentId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        if (isDuplicateKeyError(q.lastError()))
            return ServiceResult::failure(QStringLiteral("Sinh viên đã có trong môn học này."));
        qWarning() << "enrollStudent thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể thêm sinh viên vào môn học. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Đã thêm sinh viên vào môn học."), lastInsertId(db));
}

ServiceResult CourseService::unenrollStudent(int courseId, int studentId)
{
    QSqlDatabase db = serviceDb();
    if (!db.isOpen())
        return ServiceResult::failure(QStringLiteral("Không kết nối được cơ sở dữ liệu."));

    QSqlQuery find(db);
    find.prepare(QStringLiteral(
        "SELECT 1 FROM enrollments WHERE user_id = ? AND course_id = ?"));
    find.addBindValue(studentId);
    find.addBindValue(courseId);
    if (!find.exec() || !find.next())
        return ServiceResult::failure(QStringLiteral("Sinh viên chưa ghi danh vào môn học này."));

    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "DELETE FROM enrollments WHERE user_id = ? AND course_id = ?"));
    q.addBindValue(studentId);
    q.addBindValue(courseId);
    if (!q.exec()) {
        qWarning() << "unenrollStudent thất bại:" << q.lastError().text();
        return ServiceResult::failure(QStringLiteral("Không thể xóa sinh viên khỏi môn học. Vui lòng thử lại."));
    }
    return ServiceResult::success(QStringLiteral("Đã xóa sinh viên khỏi môn học."));
}
