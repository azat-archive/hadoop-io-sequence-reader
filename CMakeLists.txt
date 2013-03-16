cmake_minimum_required(VERSION 2.6)
project(reader CXX)

set(CMAKE_C_FLAGS "-Wall -Werror -std=c++11")
set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++11")

message(STATUS "Making ${PROJECT_NAME} on ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(boostcached ${SOURCES})
