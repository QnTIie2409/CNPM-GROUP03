cmake_minimum_required(VERSION 3.16)
project(StudentAttendanceSystem VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Danh sách mã nguồn dự án theo cấu trúc 3 tầng
set(SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/mainwindow.ui

    # Utils
    src/utils/ConfigReader.h
    src/utils/ConfigReader.cpp
    src/utils/Logger.h
    src/utils/Logger.cpp

    # Models
    src/models/User.h
    src/models/Course.h
    src/models/AttendanceSession.h
    src/models/AttendanceRecord.h
    src/models/ScheduleEntry.h

    # Repositories (Data Access Layer)
    src/repositories/DatabaseManager.h
    src/repositories/DatabaseManager.cpp
    src/repositories/UserRepository.h
    src/repositories/UserRepository.cpp
    src/repositories/CourseRepository.h
    src/repositories/CourseRepository.cpp
    src/repositories/SessionRepository.h
    src/repositories/SessionRepository.cpp
    src/repositories/AttendanceRepository.h
    src/repositories/AttendanceRepository.cpp
    src/repositories/ScheduleRepository.h
    src/repositories/ScheduleRepository.cpp

    # Services (Business Layer)
    src/services/AuthService.h
    src/services/AuthService.cpp
    src/services/SessionService.h
    src/services/SessionService.cpp
    src/services/CheckInService.h
    src/services/CheckInService.cpp
    src/services/AttendanceService.h
    src/services/AttendanceService.cpp
)

add_executable(StudentAttendanceSystem ${SOURCES})

target_link_libraries(StudentAttendanceSystem PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)
