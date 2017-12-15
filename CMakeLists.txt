cmake_minimum_required(VERSION 3.6)
project(UniR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    UniversalRemote.cpp)

add_executable(UniR ${SOURCE_FILES})