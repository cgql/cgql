cmake_minimum_required(VERSION 3.10)
project(cgql)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include_directories(utils)
include_directories(schema)
file(GLOB SOURCES
  schema/*.cpp
  schema/GraphQLObject/*.h
  schema/GraphQLObject/*.cpp
  schema/*.h
  logger/logger.h
  logger/logger.cpp
  type/*.h
  type/*.cpp
  main.cpp
)
add_executable(cgql ${SOURCES})
