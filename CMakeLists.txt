cmake_minimum_required(VERSION 3.8)
project(multibox)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(SEND_ERROR "Insource building is disabled.")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
set(SOURCES
src/core.cpp
src/main.cpp
)

set(HEADERS
src/core.h
src/info.h
)
add_executable(multibox ${SOURCES})
target_link_libraries(multibox Qt5::Core Qt5::Gui Qt5::Widgets)