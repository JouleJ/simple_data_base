#include "server/driver/include/network.hpp"
#include "server/core/include/fdstream.hpp"
#include "server/core/include/logger.hpp"

#include <asm-generic/socket.h>
#include <cstddef>
#include <cstring>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

static const int yes = 1;

TcpStream::TcpStream(int desiredFd) : fd(desiredFd), fdInputStream(desiredFd) {}

TcpStream::~TcpStream() { close(fd); }

std::istream &TcpStream::getInputStream() { return fdInputStream; }

Server::Server(int desiredPort) : port(desiredPort) {
  int ret;
  struct addrinfo hints, *servinfo, *p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  const auto portStr = std::to_string(port);
  if ((ret = getaddrinfo(NULL, portStr.c_str(), &hints, &servinfo)) != 0) {
    const char *errorCStr = gai_strerror(ret);
    throw std::runtime_error(std::string("Failure when initializing server: ") +
                             std::string(errorCStr));
  }

  fd = -1;
  for (p = servinfo; p != NULL; p = p->ai_next) {
    getLogger().info("Trying getaddrinfo entry");

    fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (fd < 0) {
      getLogger().warning("Failed to open socket");
      continue;
    }

    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (ret < 0) {
      getLogger().warning("setsockopt routine failed");
      close(fd);
      continue;
    }

    ret = bind(fd, p->ai_addr, p->ai_addrlen);
    if (ret < 0) {
      getLogger().warning("Failed to bind socket to address");
      close(fd);
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo);

  if (fd < 0) {
    throw std::runtime_error(std::string("Failed to open socket for port ") +
                             std::to_string(port));
  }
}

void Server::listen(std::function<void(TcpStream &stream)> serve) {
  const int ret = ::listen(fd, 5);

  if (ret < 0) {
    throw std::runtime_error("Failed to begin listening on socket");
  }

  std::vector<std::thread> threads;
  const auto threadMain = [&serve](int connectionFd) -> void {
    auto tcpStream = TcpStream(connectionFd);
    serve(tcpStream);
  };

  while (true) {
    const int connectionFd = accept(fd, NULL, NULL);
    getLogger().info("New connection incoming, connectionFd=%d", connectionFd);

    threads.emplace_back(threadMain, connectionFd);
  }
}
