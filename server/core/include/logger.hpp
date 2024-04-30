#pragma once

#include "server/core/include/utils.hpp"
#include <ostream>

class LoggingLevel {
public:
  constexpr inline static int DEBUG = -1;
  constexpr inline static int INFO = 0;
  constexpr inline static int WARNING = 1;
  constexpr inline static int ERROR = 2;
};

class ILogger {
protected:
  int loggingLevel = LoggingLevel::DEBUG;

  virtual std::ostream &getOutputStream() = 0;
  virtual void writePrefix(const char *loggingLevel) = 0;
  virtual void flush() = 0;

public:
  virtual ~ILogger() = default;

  void setLoggingLevel(int desiredLoggingLevel);

  template <typename... Args>
  void debug(const char *formatString, Args... args) {
    if (loggingLevel > LoggingLevel::DEBUG) {
      return;
    }

    writePrefix("DEBUG");
    format<Args...>(getOutputStream(), formatString, args...);
    flush();
  }

  template <typename... Args>
  void info(const char *formatString, Args... args) {
    if (loggingLevel > LoggingLevel::INFO) {
      return;
    }

    writePrefix("INFO");
    format<Args...>(getOutputStream(), formatString, args...);
    flush();
  }

  template <typename... Args>
  void warning(const char *formatString, Args... args) {
    if (loggingLevel > LoggingLevel::WARNING) {
      return;
    }

    writePrefix("WAARN");
    format<Args...>(getOutputStream(), formatString, args...);
    flush();
  }

  template <typename... Args>
  void error(const char *formatString, Args... args) {
    if (loggingLevel > LoggingLevel::ERROR) {
      return;
    }

    writePrefix("ERROR");
    format<Args...>(getOutputStream(), formatString, args...);
    flush();
  }
};

ILogger &getLogger();
