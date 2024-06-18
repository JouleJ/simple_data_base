#include "server/core/include/case.hpp"
#include "server/core/include/logger.hpp"
#include "server/core/include/utils.hpp"
#include "server/driver/include/network.hpp"
#include "server/persistent/include/storage.hpp"
#include "server/query/include/ast.hpp"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

const auto storageTypeArgumentName = std::string("--storage-type");
const auto diskStorageRootArgumentName = std::string("--disk-storage-root");
const auto portArgumentName = std::string("--port");

const static std::map<std::string, CommandLineArgumentKind>
    commandLineArgumentsScheme = {
        {storageTypeArgumentName, CommandLineArgumentKind::STRING},
        {diskStorageRootArgumentName, CommandLineArgumentKind::STRING},
        {portArgumentName, CommandLineArgumentKind::INTEGER},
};

static void printCommandLineHelp() {
  std::cout << "Command line options:\n\n";
  std::cout << "\t" << storageTypeArgumentName << " memory|disk\n";
  std::cout << "\t[" << diskStorageRootArgumentName << " FPATH]\n";
  std::cout << "\t" << portArgumentName << " PORT\n";
}

static std::unique_ptr<IStorage>
buildStorage(const std::map<std::string, CommandLineArgumentValue>
                 &commandLineArgumentValues) {
  const auto storageType = getCommandLineArgumentValue<std::string>(
      commandLineArgumentValues, storageTypeArgumentName);
  if (!storageType) {
    throw std::runtime_error("Storage type was not specified");
  }

  return Case<std::string, std::unique_ptr<IStorage>>()
      .when("memory", []() { return makeMemoryStorage(); })
      .when("disk",
            [&]() {
              const auto diskStorageRoot =
                  getCommandLineArgumentValue<std::string>(
                      commandLineArgumentValues, diskStorageRootArgumentName);
              if (!diskStorageRoot) {
                throw std::runtime_error(
                    "Root path for disk storage was not specified");
              }

              return makeDiskStorage(std::filesystem::path(*diskStorageRoot));
            })
      .default_([&]() -> std::unique_ptr<IStorage> {
        throw std::runtime_error(std::string("Unknown storage type: ") +
                                 quote(*storageType));
      })
      .execute(*storageType);
}

int main(int argc, char **argv) {
  std::unique_ptr<IStorage> storage;
  std::optional<int> port;

  try {
    const auto commandLineArgumentValues =
        parseCommandLineArguments(commandLineArgumentsScheme, argc, argv);
    storage = buildStorage(commandLineArgumentValues);

    port = getCommandLineArgumentValue<int>(commandLineArgumentValues,
                                            portArgumentName);
    if (!port) {
      throw std::runtime_error("Port was not specified");
    }
  } catch (std::runtime_error &err) {
    printCommandLineHelp();

    std::cerr << "\n\nCritial error in main server thread!\n";
    std::cerr << "Cannot parse command line arguments: " << err.what() << "\n";
    std::cerr.flush();

    return 1;
  }

  const auto serve = [&](TcpStream &tcpStream) -> void {
    const uint64_t connectionId = splitmix64(getCurrentEpoch());
    getLogger().info("Assigned following connectionId to new connection: %h",
                     connectionId);

    NodeDeserializer nodeDeserializer(tcpStream.getInputStream());
    nodeDeserializer.forEach(
        [&](size_t queryId, std::unique_ptr<INode> node) -> void {
          getLogger().info("Got query, connectionId = %h, queryId = %h",
                           connectionId, queryId);
        });
  };

  try {
    auto server = Server(*port);
    server.listen(serve);
  } catch (std::runtime_error &err) {
    std::cerr << "Criticl error in main server thread!\n";
    std::cerr << "Shutting server down due to following error: " << err.what()
              << "\n";
    std::cerr.flush();

    return 2;
  }

  return 0;
}
