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
    const std::filesystem::path sourceFile{argv[1]};
    const std::filesystem::path targetFile{
        std::filesystem::path{sourceFile}.replace_filename(
            std::string("protected-") + sourceFile.filename().string())};

    (void)remove(targetFile);
    std::filesystem::copy(argv[1], targetFile.string(),
                          // overwrite doen't work with MinGw gcc
                          // std::filesystem::copy_options::overwrite_existing,
                          ec);
    if (!ec) {
      try {
        const NPeProtector::SClientFile clientFile =
            NPeProtector::getPeFileInfo(targetFile.string().c_str());
        // test
        NPeProtector::gImageBase = clientFile.mImageBase;

        std::ofstream fileStream(targetFile.string(),
                                 std::ios_base::binary | std::ios_base::trunc);

        if (fileStream.is_open()) {
          LOG_INITIALIZE(std::string(argv[0]) + ".log");

          NPeProtector::protectPe(fileStream, clientFile);
        } else {
          printf("Failed to open file %s", targetFile.string().c_str());
          exitCode = 1;
        }
      } catch (const std::runtime_error& e) {
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
