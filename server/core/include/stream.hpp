#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

template <typename T> class IStream {
public:
  virtual ~IStream() = default;

  virtual bool hasNext() = 0;
  virtual T getNext() = 0;

  void forEach(const std::function<void(size_t, T)> &action) {
    size_t index = 0U;
    while (hasNext()) {
      action(index++, getNext());
    }
  }
};

template <typename T> class VectorStream : public IStream<const T &> {
  using Iter = typename std::vector<T>::const_iterator;
  Iter begin, end;

public:
  VectorStream(Iter desiredBegin, Iter desiredEnd)
      : begin(desiredBegin), end(desiredEnd) {}
  ~VectorStream() = default;

  bool hasNext() override { return begin != end; }

  const T &getNext() override { return *(begin++); }
};

template <typename T> std::optional<T> fetchOne(IStream<T> &stream) {
  if (!stream.hasNext()) {
    return {};
  }

  return stream.getNext();
}

template <typename T> std::vector<T> fetchAll(IStream<T> &stream) {
  std::vector<T> result;
  while (stream.hasNext()) {
    result.emplace_back(std::forward<T>(stream.getNext()));
  }

  return result;
}
