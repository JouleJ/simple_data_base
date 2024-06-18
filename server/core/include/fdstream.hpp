#pragma once

#include <istream>
#include <streambuf>

class FdInputBuffer : public std::streambuf {
  static const int bufferSize = 12 * 1024;

  int fd;
  char buffer[bufferSize];

public:
  FdInputBuffer(int desiredFd);
  ~FdInputBuffer() override = default;

protected:
  int_type underflow() override;
};

class FdInputStream : public std::istream {
  FdInputBuffer fdInputBuffer;

public:
  FdInputStream(int desiredFd);
  ~FdInputStream() override = default;
};
