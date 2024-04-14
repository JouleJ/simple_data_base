#include "server/core/include/meta.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/errors.hpp"

#include <cstdint>
#include <memory>
#include <set>
#include <string>

class TestCaseHashFunctionProducesNoCollisions : public TestCase {
  int from, to;

public:
  TestCaseHashFunctionProducesNoCollisions(int desiredFrom, int desiredTo)
      : from(desiredFrom), to(desiredTo) {}

  std::string getName() const override {
    return std::string("hash/collision/") + std::to_string(from) +
           std::string("..") + std::to_string(to);
  }

  void run() override {
    std::set<uint64_t> hashes;

    for (int value = from; value <= to; ++value) {
      const uint64_t hash = std::make_unique<IntegerValue>(value)->getHash();
      hashes.insert(hash);
    }

    assertEqualityByVal(static_cast<int>(hashes.size()), to - from + 1);
  }
};

const int _ = []() -> int {
  registerTestCase(
      std::make_unique<TestCaseHashFunctionProducesNoCollisions>(1, 1000));
  registerTestCase(std::make_unique<TestCaseHashFunctionProducesNoCollisions>(
      5'000'000, 7'000'000));

  return 0;
}();
