cmake_minimum_required(VERSION 3.10)
project(cgql)

set(SOURCE_FILES
  main.cpp
  utils/pre_compiled_headers.h
)
add_subdirectory(parser)

add_executable(cgql ${SOURCE_FILES})
target_link_libraries(cgql parser)