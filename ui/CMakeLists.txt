cmake_minimum_required(VERSION 3.16)
project(StudentAttendanceSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Svg: icon cua giao dien. Sql/Network: de nhom khong phai sua khi phan backend can den
# (module thua khong gay loi).
find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Sql Network)

# Tu dong lay moi file .cpp/.h/.ui trong src/ va ui/
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.ui
    ${CMAKE_SOURCE_DIR}/ui/*.cpp
    ${CMAKE_SOURCE_DIR}/ui/*.h
    ${CMAKE_SOURCE_DIR}/ui/*.ui
)

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    resources/resources.qrc
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets Qt6::Svg Qt6::Sql Qt6::Network
)
