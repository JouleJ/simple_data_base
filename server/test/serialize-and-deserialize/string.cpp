#include "server/core/include/deserializer.hpp"
#include "server/core/include/serialize.hpp"
#include "server/core/include/utils.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/errors.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>

class TestCaseSerializeDeserializeString : public TestCase {
  std::string s;

public:
  TestCaseSerializeDeserializeString(std::string desired)
      : s(std::move(desired)) {}

  std::string getName() const override {
    return std::string("serialize-and-deserialize/string/") + quote(s);
  }

  void run() override {
    std::stringstream buf;
    serializeToStream(s, buf);

    const std::string parsedValue = deserializeString(buf);
    assertEqualityByVal(s, parsedValue);
  }
};

const int _ = []() -> int {
  std::string nullString(3, '\0');

  registerTestCase(
      std::make_unique<TestCaseSerializeDeserializeString>("hello world"));
  registerTestCase(std::make_unique<TestCaseSerializeDeserializeString>(""));
  registerTestCase(
      std::make_unique<TestCaseSerializeDeserializeString>(nullString));
  registerTestCase(
      std::make_unique<TestCaseSerializeDeserializeString>("123\n\t\rhello"));

  return 0;
}();
