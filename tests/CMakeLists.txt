project(cgqlTests)

add_executable(
  cgqlTests
  ./basicTest.h
  ./advancedParsing.h
  ./test.cpp
)

target_link_libraries(
  cgqlTests
  cgqlSource
)
