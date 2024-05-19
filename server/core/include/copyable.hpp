#pragma once

#include <memory>

template <typename T> class ICopyable {
public:
  ~ICopyable() = default;

  virtual std::unique_ptr<T> copy() const = 0;
};
