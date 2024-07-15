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

class FdOutputBuffer : public std::streambuf {
  static const int bufferSize = 12 * 1024;

  int fd;
  char buffer[bufferSize];

public:
  FdOutputBuffer(int desiredFd);
  ~FdOutputBuffer() override = default;

protected:
  int_type overflow(int_type chr) override;
};

class FdOutputStream : public std::ostream {
  FdOutputBuffer fdOutputBuffer;

public:
  FdOutputStream(int desiredFd);
  ~FdOutputStream() override = default;
};
