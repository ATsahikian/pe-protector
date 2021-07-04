#include "Parser.h"

#include "common/SCommand.h"
#include "common/Types.h"
#include "log/CLog.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iosfwd>
#include <iostream>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>

/**
 * @brief Compile.exe file.asm file.bin
 */
int main(int argc, char* argv[], char* env[]) {
  int exitCode = 0;
  if (argc == 3) {
    LOG_INITIALIZE(std::string(argv[0]) + ".log");
    try {
      std::ifstream file(argv[1]);
      if (file.is_open()) {
        const std::vector<NPeProtector::SCommand>& commands =
            NPeProtector::parse(file);

        std::ofstream binFile(argv[2], std::ios_base::binary);

        NPeProtector::serialize(binFile, commands);
      } else {
        exitCode = 1;
        printf("error: can't open file %s", argv[1]);
      }
    } catch (const std::runtime_error& e) {
      std::cout << e.what();
    }
  } else {
    printf("Compile.exe file.asm file.bin");
  }

  return exitCode;
}
