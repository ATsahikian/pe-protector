#include "ClientFile.h"
#include "PeHeader.h"
#include "ProtectPe.h"

#include "common/SCommand.h"
#include "log/CLog.h"

#include <filesystem>
#include <fstream>
#include <ios>
#include <ostream>

/**
 * @brief pe-protector.exe fileName
 */
int main(int argc, char* argv[], char* env[]) {
  int exitCode = 0;
  if (argc == 2) {
    std::error_code ec{};
    std::filesystem::copy(argv[1], (argv[1] + std::string(".bak")).c_str(), ec);
    // TODO: check
    if (!ec) {
      try {
        const NPeProtector::SClientFile clientFile =
            NPeProtector::getPeFileInfo(argv[1]);
        // test
        NPeProtector::gImageBase = clientFile.mImageBase;

        std::ofstream fileStream(argv[1],
                                 std::ios_base::binary | std::ios_base::trunc);

        if (fileStream.is_open()) {
          LOG_INITIALIZE(std::string(argv[0]) + ".log");

          NPeProtector::protectPe(fileStream, clientFile);
        } else {
          printf("Failed to open file %s", argv[1]);
          exitCode = 1;
        }
      } catch (const std::exception& e) {
        printf("Failed to protect file, %s", e.what());
        exitCode = 1;
      }
    } else {
      printf("Failed to copy file, %s", argv[1]);
      exitCode = 1;
    }
  } else {
    printf("pe-protector.exe fileName");
  }
  return exitCode;
}
