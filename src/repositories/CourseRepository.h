#ifndef COURSEREPOSITORY_H
#define COURSEREPOSITORY_H

#include "../models/Course.h"
#include "../models/User.h"
#include <optional>
#include <vector>

class CourseRepository {
public:
    std::optional<Course> findById(int courseId);
    std::optional<Course> findByCode(const QString& code);
    std::vector<Course> findAll();
    std::vector<Course> findByLecturer(int lecturerUserId);
    std::vector<Course> findByStudent(int studentUserId);

    bool createCourse(const Course& course);
    bool updateCourse(const Course& course);
    bool deleteCourse(int courseId);
    bool hasSessions(int courseId);

    bool enrollStudent(int studentUserId, int courseId);
    bool removeEnrollment(int studentUserId, int courseId);
    bool isStudentEnrolled(int studentUserId, int courseId);
    std::vector<User> getEnrolledStudents(int courseId);
};

#endif
