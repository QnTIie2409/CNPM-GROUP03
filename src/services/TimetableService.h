#ifndef TIMETABLESERVICE_H
#define TIMETABLESERVICE_H

#include "../models/ScheduleEntry.h"
#include "../repositories/ScheduleRepository.h"
#include "ServiceCommon.h"
#include <vector>

class TimetableService {
public:
    static TimetableService& instance();

    std::vector<ScheduleEntry> getLecturerTimetable(int lecturerUserId);
    std::vector<ScheduleEntry> getStudentTimetable(int studentUserId);
    std::vector<ScheduleEntry> getCourseTimetable(int courseId);
    std::vector<ScheduleEntry> getAllSchedules();

    ServiceResult addSchedule(const ScheduleEntry& entry);
    ServiceResult deleteSchedule(int scheduleId);

private:
    TimetableService() = default;
    ScheduleRepository m_repo;
};

#endif // TIMETABLESERVICE_H
