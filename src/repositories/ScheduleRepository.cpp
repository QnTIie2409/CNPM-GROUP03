#include "ScheduleRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>

std::vector<ScheduleEntry> ScheduleRepository::findByCourse(int courseId) {
    std::vector<ScheduleEntry> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT s.schedule_id, s.course_id, s.type, s.day_of_week, s.start_time, s.end_time, "
                  "s.room, s.start_date, s.end_date, c.code as course_code, c.name as course_name "
                  "FROM schedule_entries s "
                  "JOIN courses c ON s.course_id = c.course_id "
                  "WHERE s.course_id = :cid");
    query.bindValue(":cid", courseId);
    if (query.exec()) {
        while (query.next()) {
            ScheduleEntry e;
            e.scheduleId = query.value("schedule_id").toInt();
            e.courseId = query.value("course_id").toInt();
            e.type = (query.value("type").toString() == "CLASS") ? ScheduleType::CLASS : ScheduleType::STUDY;
            e.dayOfWeek = query.value("day_of_week").toInt();
            e.startTime = query.value("start_time").toTime();
            e.endTime = query.value("end_time").toTime();
            e.room = query.value("room").toString();
            e.startDate = query.value("start_date").toDate();
            e.endDate = query.value("end_date").toDate();
            e.courseCode = query.value("course_code").toString();
            e.courseName = query.value("course_name").toString();
            list.push_back(e);
        }
    }
    return list;
}

std::vector<ScheduleEntry> ScheduleRepository::findByStudent(int studentUserId) {
    std::vector<ScheduleEntry> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT s.schedule_id, s.course_id, s.type, s.day_of_week, s.start_time, s.end_time, "
                  "s.room, s.start_date, s.end_date, c.code as course_code, c.name as course_name "
                  "FROM schedule_entries s "
                  "JOIN courses c ON s.course_id = c.course_id "
                  "JOIN enrollments e ON c.course_id = e.course_id "
                  "WHERE e.user_id = :uid");
    query.bindValue(":uid", studentUserId);
    if (query.exec()) {
        while (query.next()) {
            ScheduleEntry e;
            e.scheduleId = query.value("schedule_id").toInt();
            e.courseId = query.value("course_id").toInt();
            e.type = (query.value("type").toString() == "CLASS") ? ScheduleType::CLASS : ScheduleType::STUDY;
            e.dayOfWeek = query.value("day_of_week").toInt();
            e.startTime = query.value("start_time").toTime();
            e.endTime = query.value("end_time").toTime();
            e.room = query.value("room").toString();
            e.startDate = query.value("start_date").toDate();
            e.endDate = query.value("end_date").toDate();
            e.courseCode = query.value("course_code").toString();
            e.courseName = query.value("course_name").toString();
            list.push_back(e);
        }
    }
    return list;
}

std::vector<ScheduleEntry> ScheduleRepository::findByLecturer(int lecturerUserId) {
    std::vector<ScheduleEntry> list;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT s.schedule_id, s.course_id, s.type, s.day_of_week, s.start_time, s.end_time, "
                  "s.room, s.start_date, s.end_date, c.code as course_code, c.name as course_name "
                  "FROM schedule_entries s "
                  "JOIN courses c ON s.course_id = c.course_id "
                  "WHERE c.user_id = :uid");
    query.bindValue(":uid", lecturerUserId);
    if (query.exec()) {
        while (query.next()) {
            ScheduleEntry e;
            e.scheduleId = query.value("schedule_id").toInt();
            e.courseId = query.value("course_id").toInt();
            e.type = (query.value("type").toString() == "CLASS") ? ScheduleType::CLASS : ScheduleType::STUDY;
            e.dayOfWeek = query.value("day_of_week").toInt();
            e.startTime = query.value("start_time").toTime();
            e.endTime = query.value("end_time").toTime();
            e.room = query.value("room").toString();
            e.startDate = query.value("start_date").toDate();
            e.endDate = query.value("end_date").toDate();
            e.courseCode = query.value("course_code").toString();
            e.courseName = query.value("course_name").toString();
            list.push_back(e);
        }
    }
    return list;
}

bool ScheduleRepository::createEntry(const ScheduleEntry& entry) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO schedule_entries (course_id, type, day_of_week, start_time, end_time, room, start_date, end_date) "
                  "VALUES (:cid, :type, :dow, :start_t, :end_t, :room, :start_d, :end_d)");
    query.bindValue(":cid", entry.courseId);
    query.bindValue(":type", (entry.type == ScheduleType::CLASS) ? "CLASS" : "STUDY");
    query.bindValue(":dow", entry.dayOfWeek);
    query.bindValue(":start_t", entry.startTime.toString("HH:mm:ss"));
    query.bindValue(":end_t", entry.endTime.toString("HH:mm:ss"));
    query.bindValue(":room", entry.room);
    query.bindValue(":start_d", entry.startDate.toString("yyyy-MM-dd"));
    query.bindValue(":end_d", entry.endDate.toString("yyyy-MM-dd"));
    return query.exec();
}

bool ScheduleRepository::deleteEntry(int scheduleId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM schedule_entries WHERE schedule_id = :id");
    query.bindValue(":id", scheduleId);
    return query.exec();
}
