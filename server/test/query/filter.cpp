#include "server/core/include/meta.hpp"
#include "server/query/include/ast.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/errors.hpp"
#include <algorithm>
#include <memory>
#include <string>

class QueryFilterTestCase : public TestCase {
public:
  QueryFilterTestCase() = default;
  ~QueryFilterTestCase() override = default;

  std::string getName() const override { return std::string("query/filter"); }

  void run() override {
    const std::unique_ptr<IComputableNode> filterRootNode =
        std::make_unique<BinaryOperator>(
            BinaryOperatorKind::AND,
            std::make_unique<BinaryOperator>(
                BinaryOperatorKind::EQ, std::make_unique<Variable>("alpha"),
                std::make_unique<IntegerLiteral>(13)),
            std::make_unique<BinaryOperator>(
                BinaryOperatorKind::LT, std::make_unique<Variable>("beta"),
                std::make_unique<IntegerLiteral>(0)));

    const Type *integerType = getPrimitiveTypeByName("integer");

    const std::vector<Column> columns = {
        Column("alpha", integerType),
        Column("beta", integerType),
    };

    TableContext tc(columns);

    {
      Row row;
      row.append(std::make_unique<IntegerValue>(13));
      row.append(std::make_unique<IntegerValue>(-1));

      assertTrue(tc.inferFilter(filterRootNode.get(), &row));
    }

    {
      Row row;
      row.append(std::make_unique<IntegerValue>(11));
      row.append(std::make_unique<IntegerValue>(-1));

      assertFalse(tc.inferFilter(filterRootNode.get(), &row));
    }

    {
      Row row;
      row.append(std::make_unique<IntegerValue>(13));
      row.append(std::make_unique<IntegerValue>(1));

      assertFalse(tc.inferFilter(filterRootNode.get(), &row));
    }
  }
};

const int _ = []() -> int {
  registerTestCase(std::make_unique<QueryFilterTestCase>());

  return 0;
}();
