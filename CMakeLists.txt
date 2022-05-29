cmake_minimum_required(VERSION 3.10)
project(cgql)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CPP_SOURCES
  src/cgql.cpp
  src/execute/execute.cpp
  src/execute/selectionSetExecutor.cpp
  src/lexer/tokenizer.cpp
  src/parser/baseParser.cpp
  src/parser/parser.cpp
  src/parser/schemaParser.cpp
  src/query/Document.cpp
  src/schema/typeRegistry.cpp
  src/schema/typeDefinitions.cpp
)

add_library(cgqlSource)
add_library(cgql::cgqlSource ALIAS cgqlSource)

target_include_directories(cgqlSource
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)

target_sources(cgqlSource PRIVATE ${CPP_SOURCES})

add_subdirectory(tests)
