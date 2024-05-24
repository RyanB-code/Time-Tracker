cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(C_STANDARD_REQUIRED True)

set (EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/")

project(TimeTracker)

# For the exe name
set(EXE_NAME "time-tracker")

add_subdirectory(src)
add_subdirectory(include)

