#pragma once

#include "common/SCommand.h"

#include <vector>

namespace NPeProtector {
/**
 * @brief Mutate all assembler commands
 */
void mutateCommands(std::vector<SCommand>& commands);
}  // namespace NPeProtector