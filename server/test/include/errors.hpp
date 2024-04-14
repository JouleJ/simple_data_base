#pragma once

#include <exception>

class BaseUnitTestError : public std::exception {
public:
  virtual ~BaseUnitTestError() noexcept override = default;

  virtual const char *what() const noexcept override = 0;
};

class EqualityAssertionFailedError : public BaseUnitTestError {
public:
  ~EqualityAssertionFailedError() noexcept override = default;

  const char *what() const noexcept override;
};

template <typename T> void assertEqualityByVal(T lhs, T rhs) {
  if (lhs != rhs) {
    throw EqualityAssertionFailedError();
  }
}

template <typename T> void assertEqualityByRef(const T &lhs, const T &rhs) {
  if (lhs != rhs) {
    throw EqualityAssertionFailedError();
  }
}
