cmake_minimum_required(VERSION 3.10)
project(zerobus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic -O2)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED libzmq)

include_directories(${ZMQ_INCLUDE_DIRS})

add_executable(publisher src/publisher/main.cpp)
target_link_libraries(publisher ${ZMQ_LIBRARIES})

add_executable(consumer src/consumer/main.cpp)
target_link_libraries(consumer ${ZMQ_LIBRARIES})
