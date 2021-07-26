#define BOOST_TEST_MODULE instruction test
#include <boost/test/included/unit_test.hpp>

#include "pe-protector/Instruction.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(InstructionTest);

BOOST_AUTO_TEST_CASE(testGetInstructionSizeNOP) {
  SInstruction instruction{NPrefix::NON, NInstruction::NOP,
                           std::vector<SOperand>{}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 1);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizeADD_REG8_IMM8) {
  SOperand operand1;
  operand1.mType = NOperand::REG8;
  operand1.mRegister = NRegister::CH;

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 3);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizeADD_MEM8_IMM81) {
  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EDX}, 0, NSegment::NON, SConstant{}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 3);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizeADD_MEM8_IMM82) {
  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP},
                             0,
                             NSegment::NON,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 7);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizeADD_MEM8_IMM83) {
  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP, NRegister::EAX},
                             0,
                             NSegment::NON,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 8);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizeADD_MEM8_IMM84) {
  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP, NRegister::EAX},
                             2,
                             NSegment::FS,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 9);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizeADD_MEM8_IMM85) {
  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP, NRegister::EAX},
                             2,
                             NSegment::FS,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 9);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizeADD_REG32_REG32) {
  SOperand operand1;
  operand1.mType = NOperand::REG32;
  operand1.mRegister = NRegister::EAX;

  SOperand operand2;
  operand2.mType = NOperand::REG32;
  operand2.mRegister = NRegister::EBP;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 2);
}

BOOST_AUTO_TEST_CASE(testGetInstructionSizePrefix) {
  SOperand operand1;
  operand1.mType = NOperand::REG32;
  operand1.mRegister = NRegister::EAX;

  SOperand operand2;
  operand2.mType = NOperand::REG32;
  operand2.mRegister = NRegister::EBP;

  SInstruction instruction{
      NPrefix::REPZ, NInstruction::ADD, {operand1, operand2}};

  const int size = getInstructionSize(instruction);

  BOOST_TEST(size == 3);
}

BOOST_AUTO_TEST_CASE(testPutInstructionNOP) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x90};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SInstruction instruction{NPrefix::NON, NInstruction::NOP,
                           std::vector<SOperand>{}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionADD_REG8_IMM8) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x80, 0xC5, 0x88};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::REG8;
  operand1.mRegister = NRegister::CH;

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionADD_MEM8_IMM81) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x80, 0x02, 0x88};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EDX}, 0, NSegment::NON, SConstant{}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionADD_MEM8_IMM82) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x80, 0x85, 0xFF, 0xFF, 0x00, 0x00, 0x88};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP},
                             0,
                             NSegment::NON,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionADD_MEM8_IMM83) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x80, 0x84, 0x28, 0xFF, 0xFF, 0x00, 0x00, 0x88};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP, NRegister::EAX},
                             0,
                             NSegment::NON,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionADD_MEM8_IMM84) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x64, 0x80, 0x84, 0x68, 0xFF,
                                0xFF, 0x00, 0x00, 0x88};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP, NRegister::EAX},
                             2,
                             NSegment::FS,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionADD_MEM8_IMM85) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x64, 0x80, 0x84, 0x68, 0xFF,
                                0xFF, 0x00, 0x00, 0x88};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::MEM8;
  operand1.mMemory = SMemory{{NRegister::EBP, NRegister::EAX},
                             2,
                             NSegment::FS,
                             SConstant{std::vector<SLabel>{}, 0xFFFF}};

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionADD_REG32_REG32) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0x01, 0xE8};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::REG32;
  operand1.mRegister = NRegister::EAX;

  SOperand operand2;
  operand2.mType = NOperand::REG32;
  operand2.mRegister = NRegister::EBP;

  SInstruction instruction{
      NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutInstructionPrefix) {
  std::stringstream expectedResult;
  unsigned char characters[] = {0xF3, 0x01, 0xE8};
  expectedResult.write(reinterpret_cast<char*>(characters), sizeof(characters));

  std::stringstream result;

  SOperand operand1;
  operand1.mType = NOperand::REG32;
  operand1.mRegister = NRegister::EAX;

  SOperand operand2;
  operand2.mType = NOperand::REG32;
  operand2.mRegister = NRegister::EBP;

  SInstruction instruction{
      NPrefix::REPZ, NInstruction::ADD, {operand1, operand2}};

  putInstruction(result, instruction, std::vector<SCommand>{}, 0);

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}
BOOST_AUTO_TEST_SUITE_END();
