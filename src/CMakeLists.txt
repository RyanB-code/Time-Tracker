
add_executable(${PROJECT_NAME}  Commands.cpp 
        FileIO.cpp 
        Framework.cpp 
        main.cpp 
        Project.cpp 
        Settings.cpp 
        Timing.cpp
    )

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
