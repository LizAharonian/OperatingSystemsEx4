cmake_minimum_required(VERSION 3.8)
project(OperatingSystemsEx4)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
set(SOURCE_FILES main.c threadPool.c threadPool.h osqueue.c osqueue.h)
add_executable(OperatingSystemsEx4 ${SOURCE_FILES})