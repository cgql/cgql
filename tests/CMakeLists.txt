project(cgqlTests)

add_executable(
  cgqlTests
  ./basicExecution.h
  ./advancedParsing.h
  ./test.cpp
)

target_link_libraries(
  cgqlTests
  cgqlSource
)
