cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD            17)
set(C_STANDARD_REQUIRED         True)

set (EXECUTABLE_OUTPUT_PATH     "${CMAKE_SOURCE_DIR}/bin/")
set (EXE_NAME                   "time-tracker")

add_compile_definitions(RELEASE)

# Change the version information here
project(TimeTracker VERSION 1.5.1)

configure_file(src/Config.h.in Config.h)

add_subdirectory(src)
add_subdirectory(include)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
