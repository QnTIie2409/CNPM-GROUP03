#pragma once

#include <QString>

#include "ServiceCommon.h"

// FR-15, FR-16 | UC-14, UC-15
class CourseService
{
public:
    // Tạo môn học và phân công giảng viên phụ trách.
    // Thành công: ServiceResult::id là course_id mới.
    static ServiceResult createCourse(const QString &code, const QString &name,
                                      int lecturerId);

    // Sửa mã / tên môn học.
    static ServiceResult updateCourse(int courseId, const QString &code,
                                      const QString &name);

    // Phân công (đổi) giảng viên phụ trách môn học.
    static ServiceResult assignLecturer(int courseId, int lecturerId);

    // FR-15: môn đã có phiên điểm danh trong DB thì cấm xóa.
    static ServiceResult deleteCourse(int courseId);

    // FR-16: ghi danh - thêm / xóa sinh viên vào môn học.
    static ServiceResult enrollStudent(int courseId, int studentId);
    static ServiceResult unenrollStudent(int courseId, int studentId);
};
