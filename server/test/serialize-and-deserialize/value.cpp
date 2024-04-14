#include "server/core/include/deserializer.hpp"
#include "server/core/include/serialize.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/errors.hpp"

#include "server/core/include/meta.hpp"

#include <memory>
#include <sstream>

class SerializeAndDerializeValueTestCase : public TestCase {
  std::unique_ptr<Value> value;

public:
  SerializeAndDerializeValueTestCase(std::unique_ptr<Value> desiredValue): value(std::move(desiredValue)) {
  }

  std::string getName() const override {
    return std::string("serialize-and-deserialize/value/") + value->toString();
  }

  void run() override {
    std::stringstream buf;
    serializeToStream(*value, buf);

    const std::unique_ptr<Value> parsedValue = ValueDeserializer(buf).getNext();
    assertEqualityByRef(*value, *parsedValue);
  }
};

const int _ = []() -> int {
  registerTestCase(std::make_unique<SerializeAndDerializeValueTestCase>(std::make_unique<IntegerValue>(42)));
  registerTestCase(std::make_unique<SerializeAndDerializeValueTestCase>(std::make_unique<IntegerValue>(-1)));
  registerTestCase(std::make_unique<SerializeAndDerializeValueTestCase>(std::make_unique<IntegerValue>(0)));

  registerTestCase(std::make_unique<SerializeAndDerializeValueTestCase>(std::make_unique<VarcharValue>("hello world")));
  registerTestCase(std::make_unique<SerializeAndDerializeValueTestCase>(std::make_unique<VarcharValue>("-12\t\n ooze")));
  registerTestCase(std::make_unique<SerializeAndDerializeValueTestCase>(std::make_unique<VarcharValue>("")));

  return 0;
}();
