cmake_minimum_required(VERSION 3.16)
project(SessionLifecycleCheck LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Sql)
get_filename_component(APP_ROOT "../.." ABSOLUTE)
add_executable(session_lifecycle
    session_lifecycle.cpp
    ${APP_ROOT}/src/services/SessionService.cpp
    ${APP_ROOT}/src/repositories/SessionRepository.cpp
    ${APP_ROOT}/src/repositories/AttendanceRepository.cpp
    ${APP_ROOT}/src/repositories/DatabaseManager.cpp
    ${APP_ROOT}/src/utils/ConfigReader.cpp
    ${APP_ROOT}/src/utils/Logger.cpp)
target_include_directories(session_lifecycle PRIVATE ${APP_ROOT})
target_link_libraries(session_lifecycle PRIVATE Qt6::Core Qt6::Sql)
