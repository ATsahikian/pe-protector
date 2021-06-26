#pragma once

#include "ClientFile.h"

#include <iosfwd>

namespace NPeProtector {
/**
 * @brief Put the whole protected file in stream
 * @param[in] output output stream
 * @param[in] clientFile data of source file
 */
void protectPe(std::ostream& output, const SClientFile& clientFile);
}  // namespace NPeProtector