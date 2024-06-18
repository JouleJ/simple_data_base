#pragma once

#include <functional>
#include <map>
#include <optional>
#include <stdexcept>

template <typename T, typename Result> class Case {
  std::map<T, std::function<Result()>> branches;
  std::optional<std::function<Result()>> defaultBranch;

public:
  Case() = default;
  ~Case() = default;

  Case &when(T value, std::function<Result()> handler) {
    branches[value] = handler;
    return *this;
  }

  Case &default_(std::function<Result()> handle) {
    defaultBranch = handle;
    return *this;
  }

  Result execute(const T &value) const {
    const auto iter = branches.find(value);
    if (iter != branches.end()) {
      return (iter->second)();
    }

    if (defaultBranch) {
      return (*defaultBranch)();
    }

    throw std::runtime_error("Case::execute");
  }
};
