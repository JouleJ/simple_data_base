#include "server/core/include/fdstream.hpp"
#include <cstdio>
#include <stdexcept>
#include <unistd.h>

FdInputBuffer::FdInputBuffer(int desiredFd) : fd(desiredFd) {
  setg(&buffer[0], &buffer[0], &buffer[0]);
}

FdInputBuffer::int_type FdInputBuffer::underflow() {
  if (gptr() < egptr()) {
    return FdInputBuffer::traits_type::to_int_type(*gptr());
  }

  const int countRead = read(fd, &buffer[0], bufferSize);
  if (countRead < 0) {
    throw std::runtime_error("(FdInputBuffer::underflow) Failed to read");
  }

  if (countRead == 0) {
    return EOF;
  }

  setg(&buffer[0], &buffer[0], &buffer[countRead]);
  return FdInputBuffer::traits_type::to_int_type(*gptr());
}

FdInputStream::FdInputStream(int desiredFd) : fdInputBuffer(desiredFd) {
  rdbuf(&fdInputBuffer);
}
