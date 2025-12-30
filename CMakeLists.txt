cmake_minimum_required(VERSION 2.84)

set(CMAKE_CXX_STANDARD 11)

project(tutor)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include_directories(include)

add_definitions(-DPROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_library(libTutor SHARED
    src/tutor.cpp
)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)


add_executable(main
    src/main.cpp 
)

add_executable(simulatore src/simulatore.cpp)

target_link_libraries(main
    ${PROJECT_NAME}
    libTutor
)

install(TARGETS ${PROJECT_NAME} libTutor
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)
