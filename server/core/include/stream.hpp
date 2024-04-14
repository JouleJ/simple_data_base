#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>
template <typename T> class IStream {
public:
  virtual ~IStream() = default;

  virtual bool hasNext() = 0;
  virtual T getNext() = 0;
};

template <typename T> T fetchOne(IStream<T> &stream) {
  if (!stream.hasNext()) {
    throw std::runtime_error("Cannot fetch element from empty stream");
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
