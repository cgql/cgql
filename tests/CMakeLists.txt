project(cgqlTests)

add_executable(
  cgqlTests
  basicParsing.cpp
)

target_link_libraries(
  cgqlTests
  cgqlSource
)
