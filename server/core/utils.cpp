#include "server/core/include/utils.hpp"
#include "server/core/include/stringable.hpp"

#include <algorithm>
#include <bits/chrono.h>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

std::string str2hex(unsigned long long int value) {
  std::string result;
  while (value != 0) {
    const auto digit = value & 0x0F;
    result.push_back(HEX_DIGITS[digit]);

    value >>= 4U;
  }

  if (result.empty()) {
    result.push_back(HEX_DIGITS[0]);
  }

  std::reverse(result.begin(), result.end());
  return result;
}

std::string quote(const std::string &s) {
  std::stringstream buf;
  buf << '"';

  const size_t n = s.size();
  for (size_t i = 0; i != n; ++i) {
    const unsigned char chr = s[i];

    switch (chr) {
      case '\n':
        buf << "\\n";
        break;

      case '\t':
        buf << "\\t";
        break;

      case '\r':
        buf << "\\r";
        break;

      case '\0':
        buf << "\\0";
        break;

      case '"':
        buf << "\\\"";
        break;

      case '\\':
        buf << "\\\\";
        break;

      default:
        {
          if (MIN_ASCII_PRINTABLE <= chr && chr <= MAX_ASCII_PRINTABLE) {
            buf << chr;
          } else {
            buf << "\\x" << str2hex(chr);
          }
        }

        break;
    }
  }

  buf << '"';
  return buf.str();
}

uint64_t getCurrentEpoch() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

Formatter::Formatter(const char *desiredFormatString, std::ostream &desiredOs)
    : formatString(desiredFormatString), os(desiredOs) {}

Formatter::~Formatter() {
  while (seek()) {
  }
}

int Formatter::seek() {
  while (*formatString) {
    if (*formatString == '%') {
      ++formatString;

      if (*formatString != '%') {
        if (*formatString == '\0') {
          throw std::runtime_error("Trailing percent sign in format string");
        }

        return *formatString++;
      } else {
        os.write(formatString++, 1);
      }
    } else {
      os.write(formatString++, 1);
    }
  }

  return 0;
}

void Formatter::accept(int i) {
  const int seekResult = seek();
  switch (seekResult) {
    case '\0':
      throw std::runtime_error("Too many arguments in a format string");

    case 's':
    case 'd':
    case 'v':
      os << i;
      break;

    case 'h':
      os << str2hex(i);
      break;

    default:
      throw std::runtime_error(
          std::string("Invalid format specifier for integer: ") +
          std::string(1, seekResult));
  }
}

void Formatter::accept(size_t size) {
  const int seekResult = seek();
  switch (seekResult) {
    case '\0':
      throw std::runtime_error("Too many arguments for this format string");

    case 's':
    case 'd':
    case 'v':
      os << size;
      break;

    case 'h':
      os << str2hex(size);
      break;

    default:
      throw std::runtime_error(
          std::string("Invalid format specifier for size_t: ") +
          std::string(1, seekResult));
  }
}

void Formatter::accept(const std::string &string) {
  const int seekResult = seek();
  switch (seekResult) {
    case '\0':
      throw std::runtime_error("Too many arguments for this format string");

    case 's':
    case 'v':
      os << string;
      break;

    case 'q':
      os << quote(string);
      break;

    default:
      throw std::runtime_error(
          std::string("Invalid format specifier for string: ") +
          std::string(1, seekResult));
  }
}

void Formatter::accept(const IStringable &object) {
  const int seekResult = seek();
  switch (seekResult) {
    case '\0':
      throw std::runtime_error("Too many arguments for this format string");

    case 's':
    case 'o':
    case 'v':
      os << object.toString();
      break;

    default:
      throw std::runtime_error(
          std::string("Invalid format specifier for object: ") +
          std::string(1, seekResult));
  }
}

void Formatter::accept(const Type *type) {
  const int seekResult = seek();
  switch (seekResult) {
    case '\0':
      throw std::runtime_error("Too many arguments for this format string");

    case 's':
    case 'v':
    case 't':
      os << type->getName();
      break;

    default:
      throw std::runtime_error(
          std::string("Invalid format specifier for type: ") +
          std::string(1, seekResult));
  }
}

void Formatter::formatSequenceElement(int i) { os << i; }

void Formatter::formatSequenceElement(size_t size) { os << size; }

void Formatter::formatSequenceElement(const std::string &string) {
  os << quote(string);
}

void Formatter::formatSequenceElement(const IStringable &object) {
  os << object.toString();
}

void Formatter::formatSequenceElement(const Type *type) {
  os << type->getName();
}

void sendToFormatter(Formatter &formatter) {}

std::map<std::string, CommandLineArgumentValue>
parseCommandLineArguments(std::map<std::string, CommandLineArgumentKind> scheme,
                          int argc, char **argv) {
  std::map<std::string, CommandLineArgumentValue> result;
  for (const auto &[argName, argKind] : scheme) {
    if (argKind == CommandLineArgumentKind::FLAG) {
      result[argName] = CommandLineArgumentValue(false);
    }
  }

  for (int i = 1; i < argc; ++i) {
    const auto argName = std::string(argv[i]);
    const auto iter = scheme.find(argName);

    if (iter == scheme.end()) {
      throw std::runtime_error(std::string("Unknown argument: ") + argName);
    }

    const auto argKind = iter->second;
    switch (argKind) {
      case CommandLineArgumentKind::FLAG:
        result[argName] = CommandLineArgumentValue(true);
        break;

      case CommandLineArgumentKind::STRING:
        if (i + 1 >= argc) {
          throw std::runtime_error(std::string("String value expected after ") +
                                   argName +
                                   std::string(" but nothing is found"));
        }

        result[argName] = CommandLineArgumentValue(std::string(argv[++i]));
        break;

      case CommandLineArgumentKind::INTEGER:
        if (i + 1 >= argc) {
          throw std::runtime_error(
              std::string("Integer value expected after ") + argName +
              std::string(" but nothing is found"));
        }

        const auto argValue = std::string(argv[++i]);
        try {
          result[argName] = CommandLineArgumentValue(std::stoi(argValue));
        } catch (const std::invalid_argument &err) {
          throw std::runtime_error(
              std::string("Cannot convert argument value ") + quote(argValue) +
              std::string(" into integer"));
        } catch (const std::out_of_range &err) {
          throw std::runtime_error(
              std::string("Argument value ") + argValue +
              std::string(" out of range for integer type"));
        }

        break;
    }
  }

  return result;
}
