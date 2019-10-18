cmake_minimum_required(VERSION 3.15)
project(speedtest C)

set(CMAKE_C_STANDARD 99)

include_directories(include)
if (CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
    link_libraries(pthread)
    link_libraries(m)
endif ()
set(SRC)
AUX_SOURCE_DIRECTORY(./speed SRC)
add_executable(speedtest ${SRC})