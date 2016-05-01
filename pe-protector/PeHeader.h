#pragma once

#include "ClientFile.h"

#include "common/SCommand.h"

#include <iosfwd>

namespace NPeProtector {
const int gSectionAlignment = 0x1000;
const int gFileAlignment = 0x200;
extern int gImageBase;

/**
 * @brief Get size of pe header
 */
int getHeaderSize();

/**
 * @brief Put header in stream
 * @param[in] output output stream
 * @param[in] commands commands
 * @param[in] clientFile it's used for size of image and image base.
 */
void putHeader(std::ostream& output,
               const std::vector<SCommand>& commands,
               const SClientFile& clientFile);
}  // namespace NPeProtector