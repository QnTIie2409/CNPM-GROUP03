#pragma once

#include <QString>

#include "ServiceCommon.h"

class CourseService
{
public:
    static ServiceResult createCourse(const QString &code, const QString &name,
                                      int lecturerId);

    static ServiceResult updateCourse(int courseId, const QString &code,
                                      const QString &name);

    static ServiceResult assignLecturer(int courseId, int lecturerId);

    static ServiceResult deleteCourse(int courseId);

    static ServiceResult enrollStudent(int courseId, int studentId);
    static ServiceResult unenrollStudent(int courseId, int studentId);
};
