cmake_minimum_required(VERSION 3.16)
project(cgqlSource)

file(GLOB SOURCES
  base/*.h
  base/*.cpp
  schema/*.cpp
  schema/GraphQLObject/*.h
  schema/GraphQLObject/*.cpp
  schema/*.h
  logger/logger.h
  type/*.h
  type/*.cpp
  type/parser/*.h
  type/parser/*.cpp
  execute/*.h
  execute/*.cpp
  utilities/*.h
  cgql.h
  cgql.cpp
)
add_library(cgqlSource ${SOURCES})

target_include_directories(cgqlSource
  PUBLIC
  ../
)

target_precompile_headers(cgqlSource
  PRIVATE
  base/cgqlPch.h
)
