cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD            17)
set(C_STANDARD_REQUIRED         True)

set (EXECUTABLE_OUTPUT_PATH     "${CMAKE_SOURCE_DIR}/bin/")
set (EXE_NAME                   "time-tracker")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	option(DEBUG "DEBUG" ON)
endif()

# Change the version information here
project(TimeTracker VERSION 1.7.0)

add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h")


target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
