#pragma once

#include "ClientFile.h"

#include "common/SCommand.h"

#include <iosfwd>

namespace NPeProtector {
/**
 * @brief Get size of resources directory
 * @param[in] clientFile get icons, group icons and manifest.
 * @return Size
 */
int getResourcesSize(const SClientFile& clientFile);
/**
 * @brief Put resource directory in stream
 * @param[in] output output stream
 * @param[in] commands commands
 * @param[in] clientFile data of source file
 * @param[in] rva offset where resource directory will be
 */
void putResources(std::ostream& output,
                  const std::vector<SCommand>& commands,
                  const SClientFile& clientFile,
                  uint32_t rva);
}  // namespace NPeProtector