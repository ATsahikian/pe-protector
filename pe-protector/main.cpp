#include <filesystem>
#include <fstream>
#include <ios>
#include "../common/SCommand.h"
#include "../log/CLog.h"
#include "ClientFile.h"
#include "PeHeader.h"
#include "ProtectPe.h"
#include "ostream"

using std::exception;
using std::ios_base;
using std::ofstream;
using std::string;

/**
 * @brief pe-protector.exe fileName
 */
int main(int argc, char* argv[], char* env[]) {
  int exitCode = 0;
  if (argc == 2) {
    std::error_code ec{};
    std::filesystem::copy(argv[1], (argv[1] + string(".bak")).c_str(), ec);
    // TODO: check
    if (!ec) {
      try {
        const NPeProtector::SClientFile clientFile =
            NPeProtector::getPeFileInfo(argv[1]);
        // test
        NPeProtector::gImageBase = clientFile.mImageBase;

        ofstream fileStream(argv[1], ios_base::binary | ios_base::trunc);

        if (fileStream.is_open()) {
          LOG_INITIALIZE(string(argv[0]) + ".log");

          NPeProtector::protectPe(fileStream, clientFile);
        } else {
          printf("Failed to open file %s", argv[1]);
          exitCode = 1;
        }
      } catch (const exception& e) {
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
