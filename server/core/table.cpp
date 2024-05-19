#include "server/core/include/meta.hpp"
#include "server/core/include/serialize.hpp"

#include <ostream>
#include <stdexcept>
#include <string>

Column::Column(std::string desiredName, const Type *desiredType)
    : name(std::move(desiredName)), type(desiredType) {}

void Column::writeTo(std::ostream &os) const {
  serializeToStream(name, os);
  type->writeTo(os);
}

std::string Column::toString() const {
  return name + std::string(": ") + type->getName();
}

Table::Table(std::vector<Column> desiredColumns)
    : columns(std::move(desiredColumns)) {}

void Table::append(Row row) {
  if (!compareTypeVectors(getTypes(), row.getTypes())) {
    throw std::runtime_error("Cannot insert: incorrect type vector");
  }

  rows.emplace_back(std::move(row));
}

Row *Table::row_at(size_t idx) { return &rows.at(idx); }

size_t Table::getRowCount() const { return rows.size(); }

const Row *Table::row_at_const(size_t idx) const { return &rows.at(idx); }

const std::vector<Row> &Table::toVector() const { return rows; }

std::vector<const Type *> Table::getTypes() const {
  std::vector<const Type *> types;

  types.reserve(columns.size());
  for (const auto &column : columns) {
    types.push_back(column.type);
  }

  return types;
}
