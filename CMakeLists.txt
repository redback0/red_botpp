cmake_minimum_required(VERSION 3.22)
project(red_bot++ VERSION 0.0 DESCRIPTION "red_bot but in c++")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/commandHandler.cpp
    src/BotDatabase.cpp
    src/commands/commandTest.cpp
    src/commands/commandRepeat.cpp
    src/commands/commandPing.cpp
    src/commands/commandEco.cpp
)

find_package(DPP REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(${PROJECT_NAME}
    ${DPP_LIBRARIES}
    SQLite::SQLite3
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${DPP_LIBRARIES_DIR}
    includes
    src
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)