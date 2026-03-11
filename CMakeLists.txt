cmake_minimum_required(VERSION 3.20)
project(PravoslavniKalendar VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 QUIET COMPONENTS Widgets Multimedia MultimediaWidgets Svg)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets Svg)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(S "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(R "${CMAKE_CURRENT_SOURCE_DIR}/resources")

set(SOURCES
    "${S}/main.cpp"
    "${S}/mainwindow.cpp"
    "${S}/calendarwidget.cpp"
    "${S}/orthodoxholidays.cpp"
    "${S}/eventmanager.cpp"
    "${S}/musicplayer.cpp"
    "${R}/resources.qrc"
)

if(WIN32)
    add_executable(PravoslavniKalendar WIN32 ${SOURCES})
else()
    add_executable(PravoslavniKalendar ${SOURCES})
endif()

target_include_directories(PravoslavniKalendar PRIVATE "${S}")

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(PravoslavniKalendar
        Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets Qt6::Svg)
else()
    target_link_libraries(PravoslavniKalendar
        Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Svg)
endif()
