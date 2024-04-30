#pragma once

#include "server/core/include/stringable.hpp"
#include <cstdint>
#include <ostream>
#include <string>

const unsigned long long int MIN_ASCII_PRINTABLE = 32;
const unsigned long long int MAX_ASCII_PRINTABLE = 127;

const std::string HEX_DIGITS = "0123456789ABCEDF";

std::string str2hex(unsigned long long int value);
std::string quote(const std::string &s);

uint64_t getCurrentEpoch();
uint64_t splitmix64(uint64_t state);

class Formatter {
  const char *formatString = "";
  std::ostream &os;

  int seek();

public:
  Formatter(const char *desiredFormatString, std::ostream &desiredOs);
  ~Formatter();

  void accept(int i);
  void accept(size_t size);
  void accept(const std::string &string);
  void accept(const IStringable &object);
};

template <typename... Args>
void sendToFormatter(Formatter &formatter, Args... args);

template <typename... Args>
void sendToFormatter(Formatter &formatter, int i, Args... args) {
  formatter.accept(i);
  sendToFormatter<Args...>(formatter, args...);
}

template <typename... Args>
void sendToFormatter(Formatter &formatter, size_t size, Args... args) {
  formatter.accept(size);
  sendToFormatter<Args...>(formatter, args...);
}

template <typename... Args>
void sendToFormatter(Formatter &formatter, const std::string &string,
                     Args... args) {
  formatter.accept(string);
  sendToFormatter<Args...>(formatter, args...);
}

template <typename... Args>
void sendToFormatter(Formatter &formatter, const IStringable &object,
                     Args... args) {
  formatter.accept(object);
  sendToFormatter<Args...>(formatter, args...);
}

template <typename... Args>
void format(std::ostream &os, const char *formatString, Args... args) {
  Formatter formatter(formatString, os);
  sendToFormatter<Args...>(formatter, args...);
}
