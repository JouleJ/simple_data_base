#pragma once

#include "server/core/include/meta.hpp"
#include "server/core/include/stringable.hpp"
#include <cstdint>
#include <map>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <variant>

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

  void formatSequenceElement(int i);
  void formatSequenceElement(size_t size);
  void formatSequenceElement(const std::string &string);
  void formatSequenceElement(const IStringable &object);
  void formatSequenceElement(const Type *type);

public:
  Formatter(const char *desiredFormatString, std::ostream &desiredOs);
  ~Formatter();

  void accept(int i);
  void accept(size_t size);
  void accept(const std::string &string);
  void accept(const IStringable &object);
  void accept(const Type *type);

  template <typename T> void accept(const std::vector<T> &sequence) {
    const int seekResult = seek();
    switch (seekResult) {
      case '\0':
        throw std::runtime_error("Too many arguments for this format string");

      case 's':
      case 'v':
        break;

      default:
        throw std::runtime_error(
            std::string("Invalid format specifier for vector: ") +
            std::string(1, seekResult));
    }

    os << "[";

    const size_t n = sequence.size();
    for (size_t i = 0U; i != n; ++i) {
      if (i != 0U) {
        os << ", ";
      }

      formatSequenceElement(sequence.at(i));
    }

    os << "]";
  }
};

void sendToFormatter(Formatter &formatter);

template <typename T, typename... Args>
void sendToFormatter(Formatter &formatter, T head, Args... tail) {
  formatter.accept(head);
  sendToFormatter(formatter, tail...);
}

template <typename... Args>
void format(std::ostream &os, const char *formatString, Args... args) {
  Formatter formatter(formatString, os);
  sendToFormatter(formatter, args...);
}

enum class CommandLineArgumentKind {
  STRING,
  INTEGER,
  FLAG,
};

using CommandLineArgumentValue = std::variant<std::string, int, bool>;

std::map<std::string, CommandLineArgumentValue>
parseCommandLineArguments(std::map<std::string, CommandLineArgumentKind> scheme,
                          int argc, char **argv);

template <typename T>
std::optional<T> getCommandLineArgumentValue(
    const std::map<std::string, CommandLineArgumentValue> &parsingResult,
    const std::string &argName) {
  const auto iter = parsingResult.find(argName);
  if (iter == parsingResult.end()) {
    return {};
  }

  const T *ptr = std::get_if<T>(&(iter->second));
  if (ptr == nullptr) {
    return {};
  }

  return *ptr;
}
