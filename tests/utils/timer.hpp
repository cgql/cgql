#include <chrono>
#include <iostream>

class Timer {
public:
  Timer(std::string name = "Time taken")
    : name(name) {
    start = std::chrono::steady_clock::now();
  }
  ~Timer() {
    end = std::chrono::steady_clock::now();
    printResult();
  }
private:
  std::chrono::steady_clock::time_point start;
  std::chrono::steady_clock::time_point end;
  std::string name;

private:
  void printResult() {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout
      << name
      << ": "
      << duration.count()
      << "ms\n";
  }
};
