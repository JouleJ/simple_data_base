#include "server/test/include/errors.hpp"

const char *EqualityAssertionFailedError::what() const noexcept {
  return "Equality assertion failed";
}

const char *TruthAssertionFailedError::what() const noexcept {
  return "Truth assertion failed";
}

void assertTrue(bool condition) {
  if (!condition) {
    throw TruthAssertionFailedError();
  }
}
