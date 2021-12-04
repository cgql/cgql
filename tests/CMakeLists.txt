project(cgqlTests)

add_executable(
  cgqlTests
  main.cpp
)

target_link_libraries(
  cgqlTests
  cgqlSource
)
