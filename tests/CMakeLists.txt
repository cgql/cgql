project(cgqlTests)

add_executable(
  cgqlTests
  ./basicTest.h
  ./advancedParsing.h
  ./test.cpp
  ./utils/timer.hpp
)

target_link_libraries(
  cgqlTests
  cgqlSource
)
