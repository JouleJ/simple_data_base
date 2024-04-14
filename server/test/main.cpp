#include <iostream>
#include <map>

#include "server/test/include/case.hpp"

int main() {
  const std::map<std::string, TestCase *> allTestCases = getAllTestCases();
  for (const auto &[name, tc] : allTestCases) {
    std::cout << "[" << name << "] ";
    tc->run();

    std::cout << "OK\n";
  }

  return 0;
}
