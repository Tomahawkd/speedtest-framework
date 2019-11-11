cmake_minimum_required(VERSION 3.15)
project(speedtest C)

set(CMAKE_C_STANDARD 99)

# Search OpenSSL
set(OPENSSL_ROOT_DIR /usr/local/Cellar/openssl@1.1/1.1.1d)
include_directories(/usr/local/Cellar/openssl@1.1/1.1.1d/include)
link_directories(/usr/local/Cellar/openssl@1.1/1.1.1d/lib)
find_package(openssl REQUIRED)
#pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )

    include_directories(${OPENSSL_INCLUDE_DIRS})
    link_directories(${OPENSSL_LIBRARIES})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")

else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

include_directories(include)
if (CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
    link_libraries(pthread)
    link_libraries(m)
endif ()
set(SRC)
AUX_SOURCE_DIRECTORY(./speed SRC)
add_executable(speedtest ${SRC})
target_link_libraries(speedtest ${OPENSSL_LIBRARIES})