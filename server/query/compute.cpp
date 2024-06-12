#include "server/core/include/meta.hpp"
#include "server/query/include/ast.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

std::unique_ptr<Value> Variable::compute(ComputationContext ctx) const {
  const std::optional<size_t> columnIndex =
      ctx.tableContext->getColumnIndex(name);
  if (!columnIndex) {
    throw std::runtime_error(std::string("No such column: ") + name);
  }

  return ctx.row->at_const(*columnIndex)->copy();
}

std::unique_ptr<Value> BinaryOperator::compute(ComputationContext ctx) const {
  std::unique_ptr<Value> leftValue = left->compute(ctx);
  std::unique_ptr<Value> rightValue = right->compute(ctx);

  switch (kind) {
    case BinaryOperatorKind::EQ:
      return std::make_unique<BooleanValue>(*leftValue == *rightValue);

    case BinaryOperatorKind::NEQ:
      return std::make_unique<BooleanValue>(*leftValue != *rightValue);

    case BinaryOperatorKind::LT:
      return std::make_unique<BooleanValue>(*leftValue < *rightValue);

    case BinaryOperatorKind::GT:
      return std::make_unique<BooleanValue>(*leftValue > *rightValue);

    case BinaryOperatorKind::GEQ:
      return std::make_unique<BooleanValue>(*leftValue <= *rightValue);

    case BinaryOperatorKind::LEQ:
      return std::make_unique<BooleanValue>(*leftValue >= *rightValue);

    case BinaryOperatorKind::AND:
      return computeAnd(std::move(leftValue), std::move(rightValue));

    case BinaryOperatorKind::OR:
      return computeOr(std::move(leftValue), std::move(rightValue));

    default:
      throw std::runtime_error(std::string("Unknown binary operator kind: ") +
                               std::to_string(static_cast<int>(kind)));
  }
}
