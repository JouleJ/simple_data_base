#pragma once

#include <algorithm>
#include <exception>
#include <vector>

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

template <typename T>
void assertEqualityOfMultisets(std::vector<T> lhs, std::vector<T> rhs) {
  const size_t n = lhs.size();
  assertEqualityByVal(n, rhs.size());

  std::sort(lhs.begin(), lhs.end());
  std::sort(rhs.begin(), rhs.end());

  for (size_t i = 0U; i != n; ++i) {
    assertEqualityByRef(lhs.at(i), rhs.at(i));
  }
}
