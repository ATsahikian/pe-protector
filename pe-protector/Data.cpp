#include "Data.h"
#include "PeHeader.h"

#include <assert.h>
#include <ostream>

namespace NPeProtector {
uint32_t getConstantValue(const SConstant& constant,
                          const std::vector<SCommand>& commands) {
  uint32_t result = constant.mValue;

  for (unsigned int i = 0; i < constant.mLabels.size(); ++i) {
    if (constant.mLabels[i].mSign == NSign::PLUS) {
      result += commands[constant.mLabels[i].mIndex].mRVA + gImageBase;
    } else {
      result -= commands[constant.mLabels[i].mIndex].mRVA + gImageBase;
    }
  }
  return result;
}

int getDataSize(const SData& data) {
  assert((data.mCount > 1 && data.mConstants.size() == 1) ||
         (data.mCount == 1 && data.mConstants.size() >= 1));

  return data.mCount * data.mSizeData * data.mConstants.size();
}

void putData(std::ostream& output,
             const SData& data,
             const std::vector<SCommand>& commands) {
  assert((data.mCount > 1 && data.mConstants.size() == 1) ||
         (data.mCount == 1 && data.mConstants.size() >= 1));

  for (int i = 0; i < data.mCount; ++i) {
    for (unsigned int j = 0; j < data.mConstants.size(); ++j) {
      const uint32_t value = getConstantValue(data.mConstants[j], commands);
      switch (data.mSizeData) {
        case 1: {
          const char db = static_cast<char>(value);
          output.write((char*)&db, 1);
          break;
        }
        case 2: {
          const short dw = static_cast<short>(value);
          output.write((char*)&dw, 2);
          break;
        }
        case 4: {
          output.write((char*)&value, 4);
          break;
        }
        default:
          throw std::runtime_error("Wrong data size");
      }
    }
  }
}
}  // namespace NPeProtector
