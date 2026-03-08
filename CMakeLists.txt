cmake_minimum_required(VERSION 3.20)
project(PravoslavniKalendar VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 QUIET COMPONENTS Widgets Multimedia MultimediaWidgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
    set(QT_VERSION_MAJOR 5)
    message(STATUS "Using Qt5")
else()
    set(QT_VERSION_MAJOR 6)
    message(STATUS "Using Qt6")
endif()

# Auto-discover all source files
file(GLOB_RECURSE CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE H_SOURCES   "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE QRC_FILES   "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.qrc")

message(STATUS "Found CPP sources: ${CPP_SOURCES}")
message(STATUS "Found QRC files:   ${QRC_FILES}")

set(ALL_SOURCES ${CPP_SOURCES} ${H_SOURCES} ${QRC_FILES})

if(WIN32)
    add_executable(PravoslavniKalendar WIN32 ${ALL_SOURCES})
else()
    add_executable(PravoslavniKalendar ${ALL_SOURCES})
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(PravoslavniKalendar
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
    )
else()
    target_link_libraries(PravoslavniKalendar
        Qt5::Widgets
        Qt5::Multimedia
        Qt5::MultimediaWidgets
    )
endif()
