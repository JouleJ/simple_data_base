#include "server/core/include/meta.hpp"
#include "server/persistent/include/database.hpp"
#include "server/query/include/ast.hpp"
#include "server/test/include/case.hpp"
#include "server/test/include/context.hpp"
#include "server/test/include/errors.hpp"

#include <memory>
#include <string>

class SelectTestCase : public TestCase, public WithMemoryStorage {
  int n;

public:
  SelectTestCase(int desiredN) : n(desiredN) {}
  ~SelectTestCase() override = default;

  std::string getName() const override {
    return std::string("query/select/") + std::to_string(n);
  }

  void run() override {
    IStorage &storage = getStorage();

    initializeDataBase(storage);
    createSchemaInDataBase(storage, "schema_0");

    const Type *integerType = getPrimitiveTypeByName("integer");

    const std::vector<Column> columns = {
        Column("col_0", integerType),
        Column("col_1", integerType),
    };

    std::vector<Row> rows;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        Row row;
        row.append(std::make_unique<IntegerValue>(i));
        row.append(std::make_unique<IntegerValue>(j));

        rows.emplace_back(std::move(row));
      }
    }

    std::make_unique<CreateTableCommand>("schema_0", "table_0", columns)
        ->execute(storage);
    std::make_unique<InsertCommand>("schema_0", "table_0", rows)
        ->execute(storage);

    std::vector<std::string> columnsToSelect = {"col_1", "col_0"};
    auto selectCommand =
        std::make_unique<SelectCommand>("schema_0", "table_0", columnsToSelect);

    selectCommand->setFilter(std::make_unique<BinaryOperator>(
        BinaryOperatorKind::OR,
        std::make_unique<BinaryOperator>(BinaryOperatorKind::EQ,
                                         std::make_unique<Variable>("col_0"),
                                         std::make_unique<IntegerLiteral>(0)),
        std::make_unique<BinaryOperator>(BinaryOperatorKind::EQ,
                                         std::make_unique<Variable>("col_1"),
                                         std::make_unique<IntegerLiteral>(0))));

    auto selectResult = selectCommand->execute(storage);
    assertEqualityByVal<size_t>(selectResult.getRowCount(), 2 * n - 1);
  }
};

const int _ = []() -> int {
  for (int n : {1, 2, 5, 10, 123, 1234}) {
    registerTestCase(std::make_unique<SelectTestCase>(n));
  }

  return 0;
}();
