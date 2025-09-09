cmake_minimum_required(VERSION 3.10)
project(zerobus LANGUAGES C CXX)

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

# Ejercicios Bloque 1
add_executable(thread_counter src/exercises/thread_counter.cpp)
add_executable(pipe_parent_child src/exercises/pipe_parent_child.c)

# Sockets UNIX (AF_UNIX)
add_executable(unix_echo_server src/exercises/unix_echo_server.c)
add_executable(unix_echo_client src/exercises/unix_echo_client.c)

enable_testing()
add_executable(unit_tests tests/test_main.cpp)
target_link_libraries(unit_tests ${ZMQ_LIBRARIES} gtest pthread)
add_test(NAME unit_tests COMMAND unit_tests)
