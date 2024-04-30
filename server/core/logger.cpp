#include "server/core/include/logger.hpp"
#include "server/core/include/utils.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

class FileLogger : public ILogger {
  std::ofstream ofs;
  uint64_t startingTimestamp;

protected:
  std::ostream &getOutputStream() override { return ofs; }

  void writePrefix(const char *levelName) override {
    uint64_t elapsedSeconds = (getCurrentEpoch() - startingTimestamp) / 1000ULL;
    ofs << "[" << levelName << " - ";
    if (elapsedSeconds >= 3600) {
      ofs << (elapsedSeconds / 3600) << "h ";
      elapsedSeconds %= 3600;
    }

    if (elapsedSeconds >= 60) {
      ofs << (elapsedSeconds / 60) << "m ";
      elapsedSeconds %= 60;
    }

    ofs << elapsedSeconds << "s] ";
  }

  void flush() override { ofs.flush(); }

public:
  FileLogger() {
    std::filesystem::path logFilePath;
    size_t logFileIndex = 0;

    do {
      const std::string logFileName = std::string(".simple_data_base.") +
                                      std::to_string(logFileIndex) +
                                      std::string(".log");
      logFilePath = std::filesystem::current_path() / logFileName;
      ++logFileIndex;
    } while (std::filesystem::exists(logFilePath));

    ofs.open(logFilePath);
    if (!ofs.is_open()) {
      throw std::runtime_error(std::string("Failed to open "));
    }

    startingTimestamp = getCurrentEpoch();
  }

  ~FileLogger() override { ofs.close(); }
};

static std::unique_ptr<ILogger> defaultLogger;

ILogger &getLogger() {
  if (!defaultLogger) {
    defaultLogger = std::make_unique<FileLogger>();
  }

  return *defaultLogger;
}
