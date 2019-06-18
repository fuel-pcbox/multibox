cmake_minimum_required (VERSION 3.8)
set( CMAKE_VERBOSE_MAKEFILE ON )

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project (multibox)

#properly disable insource building, as CMAKE_DISABLE_IN_SOURCE_BUILD never works for me
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(SEND_ERROR "Insource building is disabled.")
endif()

set(SOURCES
src/common.h
src/cpu_ops_arith.h
src/cpu_ops_io.h
src/cpu_ops_jump.h
src/cpu_ops_misc.h
src/cpu_ops_mov.h
src/cpu_ops_table.h
src/cpu_ops.h
src/cpu.cpp
src/cpu.h
src/info.h
src/main.cpp
)

add_executable(multibox ${SOURCES})