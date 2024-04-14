#pragma once

#include <map>
#include <memory>
#include <string>

class TestCase {
public:
  virtual ~TestCase() = default;

  virtual std::string getName() const = 0;
  virtual void run() = 0;
};

void registerTestCase(std::unique_ptr<TestCase> tc);
std::map<std::string, TestCase *> getAllTestCases();
