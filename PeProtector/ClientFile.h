#pragma once

#include <cstdint>
#include <vector>

namespace NPeProtector {
/**
 * @brief Keeps all necessary information about source file
 */
struct SClientFile {
  /**
   * @brief Compressed source file
   */
  std::vector<char> mCompressed;
  /**
   * @brief Group icons resources
   */
  std::vector<char> mGroupIcons;
  /**
   * @brief Icons resources
   */
  std::vector<std::vector<char> > mIcons;
  /**
   * @brief Manifest resources
   */
  std::vector<char> mManifest;
  /**
   * @brief Image base of source file in memory
   */
  uint32_t mImageBase;
  /**
   * @brief Size of image in memory of source file
   */
  uint32_t mImageSize;
  /**
   * @brief Original entry point (RVA)
   */
  uint32_t mOEP;
};

/**
 * @brief Fills SClientFile from source file
 * @param[in] filename name of source file
 * @return SClientFile
 */
SClientFile getPeFileInfo(const char* filename);
}  // namespace NPeProtector
