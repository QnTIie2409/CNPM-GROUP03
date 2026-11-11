#include "CourseRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>

std::optional<Course> CourseRepository::findById(int courseId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT c.course_id, c.code, c.name, c.user_id, u.full_name as lecturer_name "
                  "FROM courses c JOIN users u ON c.user_id = u.user_id WHERE c.course_id = :id");
    query.bindValue(":id", courseId);

    if (query.exec() && query.next()) {
        Course c;
        c.courseId = query.value("course_id").toInt();
        c.code = query.value("code").toString();
        c.name = query.value("name").toString();
        c.lecturerUserId = query.value("user_id").toInt();
        c.lecturerName = query.value("lecturer_name").toString();
        return c;
    }
    return std::nullopt;
}

std::optional<Course> CourseRepository::findByCode(const QString& code) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT c.course_id, c.code, c.name, c.user_id, u.full_name as lecturer_name "
                  "FROM courses c JOIN users u ON c.user_id = u.user_id WHERE c.code = :code");
    query.bindValue(":code", code);

    if (query.exec() && query.next()) {
        Course c;
        c.courseId = query.value("course_id").toInt();
        c.code = query.value("code").toString();
        c.name = query.value("name").toString();
        c.lecturerUserId = query.value("user_id").toInt();
        c.lecturerName = query.value("lecturer_name").toString();
        return c;
    }
    return std::nullopt;
}

std::vector<Course> CourseRepository::findAll() {
    std::vector<Course> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT c.course_id, c.code, c.name, c.user_id, u.full_name as lecturer_name "
                  "FROM courses c JOIN users u ON c.user_id = u.user_id");
    if (query.exec()) {
        while (query.next()) {
            Course c;
            c.courseId = query.value("course_id").toInt();
            c.code = query.value("code").toString();
            c.name = query.value("name").toString();
            c.lecturerUserId = query.value("user_id").toInt();
            c.lecturerName = query.value("lecturer_name").toString();
            list.push_back(c);
        }
    }
    return list;
}

std::vector<Course> CourseRepository::findByLecturer(int lecturerUserId) {
    std::vector<Course> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT c.course_id, c.code, c.name, c.user_id, u.full_name as lecturer_name "
                  "FROM courses c JOIN users u ON c.user_id = u.user_id WHERE c.user_id = :uid");
    query.bindValue(":uid", lecturerUserId);
    if (query.exec()) {
        while (query.next()) {
            Course c;
            c.courseId = query.value("course_id").toInt();
            c.code = query.value("code").toString();
            c.name = query.value("name").toString();
            c.lecturerUserId = query.value("user_id").toInt();
            c.lecturerName = query.value("lecturer_name").toString();
            list.push_back(c);
        }
    }
    return list;
}

std::vector<Course> CourseRepository::findByStudent(int studentUserId) {
    std::vector<Course> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT c.course_id, c.code, c.name, c.user_id, u.full_name as lecturer_name "
                  "FROM courses c "
                  "JOIN users u ON c.user_id = u.user_id "
                  "JOIN enrollments e ON c.course_id = e.course_id "
                  "WHERE e.user_id = :uid");
    query.bindValue(":uid", studentUserId);
    if (query.exec()) {
        while (query.next()) {
            Course c;
            c.courseId = query.value("course_id").toInt();
            c.code = query.value("code").toString();
            c.name = query.value("name").toString();
            c.lecturerUserId = query.value("user_id").toInt();
            c.lecturerName = query.value("lecturer_name").toString();
            list.push_back(c);
        }
    }
    return list;
}

bool CourseRepository::createCourse(const Course& course) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO courses (code, name, user_id) VALUES (:code, :name, :uid)");
    query.bindValue(":code", course.code);
    query.bindValue(":name", course.name);
    query.bindValue(":uid", course.lecturerUserId);
    return query.exec();
}

bool CourseRepository::updateCourse(const Course& course) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE courses SET code = :code, name = :name, user_id = :uid WHERE course_id = :id");
    query.bindValue(":code", course.code);
    query.bindValue(":name", course.name);
    query.bindValue(":uid", course.lecturerUserId);
    query.bindValue(":id", course.courseId);
    return query.exec();
}

bool CourseRepository::hasSessions(int courseId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT COUNT(*) FROM attendance_sessions WHERE course_id = :id");
    query.bindValue(":id", courseId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool CourseRepository::deleteCourse(int courseId) {
    if (hasSessions(courseId)) {
        Logger::logError("Cannot delete course with existing attendance sessions (FR-15)");
        return false;
    }
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM courses WHERE course_id = :id");
    query.bindValue(":id", courseId);
    return query.exec();
}

bool CourseRepository::enrollStudent(int studentUserId, int courseId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO enrollments (user_id, course_id, enrolled_at) VALUES (:uid, :cid, CURDATE())");
    query.bindValue(":uid", studentUserId);
    query.bindValue(":cid", courseId);
    return query.exec();
}

bool CourseRepository::removeEnrollment(int studentUserId, int courseId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM enrollments WHERE user_id = :uid AND course_id = :cid");
    query.bindValue(":uid", studentUserId);
    query.bindValue(":cid", courseId);
    return query.exec();
}

bool CourseRepository::isStudentEnrolled(int studentUserId, int courseId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT COUNT(*) FROM enrollments WHERE user_id = :uid AND course_id = :cid");
    query.bindValue(":uid", studentUserId);
    query.bindValue(":cid", courseId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

std::vector<User> CourseRepository::getEnrolledStudents(int courseId) {
    std::vector<User> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT u.user_id, u.role, u.code, u.full_name, u.email, u.is_active, u.created_at "
                  "FROM users u "
                  "JOIN enrollments e ON u.user_id = e.user_id "
                  "WHERE e.course_id = :cid");
    query.bindValue(":cid", courseId);
    if (query.exec()) {
        while (query.next()) {
            User u;
            u.userId = query.value("user_id").toInt();
            u.role = User::stringToRole(query.value("role").toString());
            u.code = query.value("code").toString();
            u.fullName = query.value("full_name").toString();
            u.email = query.value("email").toString();
            u.isActive = query.value("is_active").toBool();
            u.createdAt = query.value("created_at").toDateTime();
            list.push_back(u);
        }
    }
    return list;
}
