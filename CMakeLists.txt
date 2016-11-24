cmake_minimum_required(VERSION 3.6)
project(IV104_ticktacktoe)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(IV104_ticktacktoe ${SOURCE_FILES})
