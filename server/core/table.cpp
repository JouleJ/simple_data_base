#include "server/core/include/meta.hpp"

#include <stdexcept>

Table::Table(std::vector<const Type *> desiredRowTypes)
    : rowTypes(std::move(desiredRowTypes)) {}

void Table::append(Row row) {
  if (!compareTypeVectors(rowTypes, row.getTypes())) {
    throw std::runtime_error("Cannot insert: incorrect type vector");
  }

  rows.push_back(std::move(row));
}

Row *Table::row_at(size_t idx) { return &rows.at(idx); }

size_t Table::getRowCount() const { return rows.size(); }

const Row *Table::row_at_const(size_t idx) const { return &rows.at(idx); }
