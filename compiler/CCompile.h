#pragma once

#include <iosfwd>
#include <istream>
#include "../common/SCommand.h"

namespace NPeProtector {
/**
 * @brief Compile source code into SCommand array for next processing in
 * Protector
 * @param[in] input character stream of source code
 * @return Array of SCommand
 */
std::vector<SCommand> compile(std::istream& input);
}  // namespace NPeProtector