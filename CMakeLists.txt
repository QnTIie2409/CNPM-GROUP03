cmake_minimum_required(VERSION 3.16)
project(StudentAttendanceSystem VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# Network: cần cho QPasswordDigestor (băm mật khẩu PBKDF2 trong AuthService)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Network Svg)

# Danh sách mã nguồn dự án theo cấu trúc 3 tầng
set(SOURCES
    # Utils
    src/utils/ConfigReader.h
    src/utils/ConfigReader.cpp
    src/utils/Logger.h
    src/utils/Logger.cpp

    # Application entry point and integrated UI shell
    src/main.cpp
    ui/UiLauncher.h
    ui/UiLauncher.cpp
    ui/LoginWindow.ui
    ui/LoginWindow.h
    ui/LoginWindow.cpp
    ui/shell/AppShell.h
    ui/shell/AppShell.cpp
    ui/MiniCalendarWidget.h
    ui/MiniCalendarWidget.cpp
    ui/WeeklyTimetableWidget.h
    ui/WeeklyTimetableWidget.cpp

    # Models
    src/models/User.h
    src/models/Course.h
    src/models/AttendanceSession.h
    src/models/AttendanceRecord.h
    src/models/AttendanceChangeLog.h
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
    src/services/ServiceCommon.h
    src/services/AuthService.h
    src/services/AuthService.cpp
    src/services/AccountService.h
    src/services/AccountService.cpp
    src/services/CourseService.h
    src/services/CourseService.cpp
    src/services/SessionService.h
    src/services/SessionService.cpp
    src/services/CheckInService.h
    src/services/CheckInService.cpp
    src/services/AttendanceService.h
    src/services/AttendanceService.cpp
    src/services/TimetableService.h
    src/services/TimetableService.cpp

    ui/resources/resources.qrc
)

add_executable(StudentAttendanceSystem ${SOURCES})

target_link_libraries(StudentAttendanceSystem PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Svg
)
