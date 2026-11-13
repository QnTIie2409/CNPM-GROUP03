#ifndef SCHEDULEREPOSITORY_H
#define SCHEDULEREPOSITORY_H

#include "../models/ScheduleEntry.h"
#include <vector>

class ScheduleRepository {
public:
    std::vector<ScheduleEntry> findByCourse(int courseId);
    std::vector<ScheduleEntry> findByStudent(int studentUserId);
    std::vector<ScheduleEntry> findByLecturer(int lecturerUserId);

    bool createEntry(const ScheduleEntry& entry);
    bool deleteEntry(int scheduleId);
};

#endif
