project(cgqlSource)

file(GLOB SOURCES
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
)
add_library(cgqlSource ${SOURCES})

target_include_directories(cgqlSource
  PUBLIC
  ../
)

target_precompile_headers(cgqlSource
  PRIVATE
  cgqlPch.h
)
