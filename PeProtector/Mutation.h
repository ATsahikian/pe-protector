#pragma once

#include "../Library/SCommand.h"
#include "vector"

namespace NPeProtector {
/**
 * @brief Mutate all assembler commands
 */
void mutateCommands(std::vector<SCommand>& commands);
}  // namespace NPeProtector