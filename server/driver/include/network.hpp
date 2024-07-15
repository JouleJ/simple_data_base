#pragma once

#include "server/core/include/fdstream.hpp"
#include <functional>
#include <istream>
#include <ostream>

class TcpStream {
  int fd;
  FdInputStream fdInputStream;
  FdOutputStream fdOutputStream;

public:
  TcpStream(int desiredFd);
  ~TcpStream();

  std::istream &getInputStream();
  std::ostream &getOutputStream();
};

class Server {
  int port;
  int fd;

public:
  Server(int desiredPort);
  ~Server() = default;

  void listen(std::function<void(TcpStream &stream)> serve);
};
