cmake_minimum_required(VERSION 3.10)
project(cgql)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wno-psabi")

add_subdirectory(cgql)
add_subdirectory(tests)
