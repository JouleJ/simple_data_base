#include <memory>
#include <stdexcept>

#include "server/test/include/case.hpp"

static std::map<std::string, std::unique_ptr<TestCase>> storage;

void registerTestCase(std::unique_ptr<TestCase> tc) {
  const std::string name = tc->getName();
  if (storage.find(name) != storage.end()) {
    throw std::runtime_error(std::string("Such test case already exists: ") +
                             name);
  }

  storage[name] = std::move(tc);
}

std::map<std::string, TestCase *> getAllTestCases() {
  std::map<std::string, TestCase *> result;
  for (const auto &[name, tc] : storage) {
    result[name] = tc.get();
  }

  return result;
}
