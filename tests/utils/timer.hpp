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
      << "\u001b[37m"
      << name
      << "\u001b[0m"
      << ": \u001b[37;1m\u001b[4m"
      << duration.count()
      << "\u001b[0mms\n";
  }
};
