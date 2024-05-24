include_directories(${EXE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable(${EXE_NAME}  Commands.cpp 
        FileIO.cpp 
        Framework.cpp 
        main.cpp 
        Project.cpp 
        Settings.cpp 
        Timing.cpp
    )