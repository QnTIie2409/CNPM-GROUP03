#include "TimetableService.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

TimetableService& TimetableService::instance() {
    static TimetableService inst;
    return inst;
}

std::vector<ScheduleEntry> TimetableService::getLecturerTimetable(int lecturerUserId) {
    return m_repo.findByLecturer(lecturerUserId);
}

std::vector<ScheduleEntry> TimetableService::getStudentTimetable(int studentUserId) {
    return m_repo.findByStudent(studentUserId);
}

std::vector<ScheduleEntry> TimetableService::getCourseTimetable(int courseId) {
    return m_repo.findByCourse(courseId);
}

std::vector<ScheduleEntry> TimetableService::getAllSchedules() {
    return m_repo.findAll();
}

ServiceResult TimetableService::addSchedule(const ScheduleEntry& entry) {
    if (entry.courseId <= 0) {
        return ServiceResult::failure("Invalid course ID.");
    }
    if (entry.startTime >= entry.endTime) {
        return ServiceResult::failure("Start time must be before end time.");
    }
    if (entry.startDate > entry.endDate) {
        return ServiceResult::failure("Start date must be before or equal to end date.");
    }

    if (m_repo.createEntry(entry)) {
        return ServiceResult::success("Schedule entry added successfully.");
    }
    return ServiceResult::failure("Failed to add schedule entry to database.");
}

ServiceResult TimetableService::deleteSchedule(int scheduleId) {
    if (scheduleId <= 0) {
        return ServiceResult::failure("Invalid schedule ID.");
    }

    if (m_repo.deleteEntry(scheduleId)) {
        return ServiceResult::success("Schedule entry deleted successfully.");
    }
    return ServiceResult::failure("Failed to delete schedule entry.");
}
