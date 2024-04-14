#include "server/test/include/errors.hpp"

const char *EqualityAssertionFailedError::what() const noexcept {
  return "Equality assertion failed";
}
