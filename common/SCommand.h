#pragma once

#include "Types.h"

#include <istream>
#include <vector>

namespace NPeProtector {
/**
 * @brief Contains reference to command in list<SCommand>
 */
struct SLabel {
  /**
   * @brief Label sign
   */
  NSign::EType mSign{};
  /**
   * @brief Index in list<SCommand>
   */
  std::size_t mIndex{};
};

/**
 * @brief Contains list of labels and constant value.
 * It's used in memory operand, ex: DWORD PTR [label1 - label2 + 666]
 */
struct SConstant {
  /**
   * @brief List of labels
   */
  std::vector<SLabel> mLabels{};
  /**
   * @brief Value of constant
   */
  uint32_t mValue{};
};

/**
 * @brief Describes memory operand in assembler instruction
 */
struct SMemory {
  std::vector<NRegister::EType> mRegisters{};

  int mScale{};  // 1->>2; 2->>4; 3-->>>8; scale! //TODO int

  NSegment::EType mSegment{NSegment::NON};

  SConstant mConstant{};
};

/**
 * @brief Describes operand in assembler instruction
 */
struct SOperand {
  /**
   * @brief Type of operand (register, memory or constant).
   */
  NOperand::EType mType{NOperand::NON};
  /**
   * @brief It's used when type = memory
   */
  SMemory mMemory{};
  /**
   * @brief It's used when type = register
   */
  NRegister::EType mRegister{};
  /**
   * @brief It's used when type = constant
   */
  SConstant mConstant{};
};

/**
 * @brief Describes assembler instruction
 */
struct SInstruction {
  NPrefix::EType mPrefix{NPrefix::NON};
  /**
   * @brief Type of instruction
   */
  NInstruction::EType mType{};
  /**
   * @brief Operands of instruction
   */
  std::vector<SOperand> mOperands{};
  /**
   * @brief Currently it's not used
   */
  int mTrash{};
};

/**
 * @brief Describes data in source code
 */
struct SData {
  /**
   * @brief Name of data, ex: API_ExAllocatePool dd 12345678h
   */
  std::string mName{};
  /**
   * @brief Size of single item of data. Ex: API_ExAllocatePool dd 12345678h =>
   * mSizeData = 4 bytes The total size = mSizeData * mCount
   */
  int mSizeData{};
  /**
   * @brief if data is dup vector.size == 1 and count > 1. If data.size ==
   * count then it's array.
   */
  std::vector<SConstant> mConstants{};
  /**
   * @brief Count of data
   */
  int mCount{};
};

/**
 * @brief Describes import function
 */
struct SImport {
  /**
   * @brief Dll name
   */
  std::string mDllName{};
  /**
   * @brief Function name
   */
  std::string mFunctionName{};
};

/**
 * @brief Describes pe section
 */
struct SSection {
  /**
   * @brief Name of section
   */
  std::string mName{};
  /**
   * @brief Attributes of section, see NSectionAttributes.
   */
  int mAttributes{};
};

/**
 * @brief Describes directive. It can be IMPORT_DIRECTORY, RECOURCE_DIRECTORY or
 * COMPRESSED_FILE.
 */
struct SDirective {
  /**
   * @brief Name of directive.
   * It can be IMPORT_DIRECTORY, RECOURCE_DIRECTORY or COMPRESSED_FILE.
   */
  std::string mName{};
  /**
   * @brief Size of directive
   */
  int mDirectorySize{};
};

/**
 * @brief It's the key struct.
 * It can contain: INSTRUCTION, DATA, DIRECTIVE, IMPORT, EXTERN, SECTION or END.
 */
struct SCommand {
  /**
   * @brief Type of command
   */
  NCommand::EType mType{NCommand::END};
  /**
   * @brief RVA(offset in memory)
   */
  uint32_t mRVA{};
  /**
   * @brief RAW (offset in file)
   */
  uint32_t mRAW{};
  /**
   * @brief Label of command
   */
  std::string mNameLabel{};
  /**
   * @brief Line number in source code. It's used for only for debug purpose.
   */
  int mNumberLine{};
  /**
   * @brief Instruction, if type == INSTRUCTION
   */
  SInstruction mInstruction{};
  /**
   * @brief Data, if type == DATA
   */
  SData mData{};
  /**
   * @brief Directive, if type == DIRECTIVE
   */
  SDirective mDirective{};
  /**
   * @brief Import, if type == IMPORT
   */
  SImport mImport{};
  /**
   * @brief Section, if type == SECTION
   */
  SSection mSection{};
};

/**
 * @brief Dump all command in trace file
 */
void loggingCommands(const std::vector<NPeProtector::SCommand>& commands);

void deserialize(std::istream& input, char& value);
void deserialize(std::istream& input, NRegister::EType& value);
void deserialize(std::istream& input, int& value);
void deserialize(std::istream& input, std::string& values);
void deserialize(std::istream& input, SLabel& label);
void deserialize(std::istream& input, SConstant& constant);
void deserialize(std::istream& input, SMemory& operandMemory);
void deserialize(std::istream& input, SOperand& operand);
void deserialize(std::istream& input, SInstruction& instruction);
void deserialize(std::istream& input, SSection& section);
void deserialize(std::istream& input, SImport& import);
void deserialize(std::istream& input, SData& data);
void deserialize(std::istream& input, SData& data);
void deserialize(std::istream& input, SCommand& command);

void serialize(std::ostream& output, const char value);
void serialize(std::ostream& output, const int value);
void serialize(std::ostream& output, const std::string& values);
void serialize(std::ostream& output, const SLabel& label);
void serialize(std::ostream& output, const SConstant& constant);
void serialize(std::ostream& output, const SMemory& operandMemory);
void serialize(std::ostream& output, const SOperand& operand);
void serialize(std::ostream& output, const SInstruction& instruction);
void serialize(std::ostream& output, const SSection& section);
void serialize(std::ostream& output, const SImport& import);
void serialize(std::ostream& output, const SData& data);
void serialize(std::ostream& output, const SCommand& command);

template <typename T>
void deserialize(std::istream& input, std::vector<T>& values) {
  int size;
  deserialize(input, size);

  for (int i = 0; i < size; ++i) {
    T value;
    deserialize(input, value);
    values.push_back(value);
  }
}

template <typename T>
void serialize(std::ostream& output, const std::vector<T>& values) {
  serialize(output, static_cast<int>(values.size()));

  for (unsigned int i = 0; i < values.size(); ++i) {
    serialize(output, values[i]);
  }
}
}  // namespace NPeProtector