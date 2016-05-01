#pragma once

#include <vector>

#include "common\SCommand.h"

namespace NPeProtector {
std::vector<SCommand> parse(std::istream& input);
}  // namespace NPeProtector