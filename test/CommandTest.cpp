#define BOOST_TEST_MODULE command test
#include <boost/test/included/unit_test.hpp>

#include "common/SCommand.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(CommandTest);

BOOST_AUTO_TEST_CASE(testSerializationExtern) {
  SCommand command;
  command.mType = NCommand::EXTERN;
  command.mNameLabel = "externImageBase";
  command.mData.mSizeData = 4;

  std::stringstream result;
  serialize(result, command);

  SCommand command2;
  deserialize(result, command2);

  BOOST_TEST(command.mType == command2.mType);
  BOOST_TEST(command.mNameLabel == command2.mNameLabel);
  BOOST_TEST(command.mData.mSizeData == command2.mData.mSizeData);
}

BOOST_AUTO_TEST_CASE(testSerializationImport) {
  SCommand command;
  command.mType = NCommand::IMPORT;
  command.mImport.mDllName = "KERNEL32.DLL";
  command.mImport.mFunctionName = "f3";

  std::stringstream result;
  serialize(result, command);

  SCommand command2;
  deserialize(result, command2);

  BOOST_TEST(command.mType == command2.mType);
  BOOST_TEST(command.mImport.mDllName == command2.mImport.mDllName);
  BOOST_TEST(command.mImport.mFunctionName == command2.mImport.mFunctionName);
}

BOOST_AUTO_TEST_CASE(testSerializationData) {
  SCommand command;
  command.mType = NCommand::DATA;

  std::vector<SLabel> labels;
  std::vector<SConstant> constants = {
      SConstant{labels, 33}, SConstant{labels, 34}, SConstant{labels, 35}};
  command.mData = SData{"", 4, constants, 1};

  std::stringstream result;
  serialize(result, command);

  SCommand command2;
  deserialize(result, command2);

  BOOST_TEST(command.mType == command2.mType);
  BOOST_TEST(command.mData.mSizeData == command2.mData.mSizeData);
}

BOOST_AUTO_TEST_CASE(testSerializationInstruction) {
  SCommand command;
  command.mType = NCommand::INSTRUCTION;

  SOperand operand1;
  operand1.mType = NOperand::REG8;
  operand1.mRegister = NRegister::CH;

  SOperand operand2;
  operand2.mType = NOperand::CONSTANT;
  operand2.mConstant.mValue = 0x88;

  command.mInstruction =
      SInstruction{NPrefix::NON, NInstruction::ADD, {operand1, operand2}};

  std::stringstream result;
  serialize(result, command);

  SCommand command2;
  deserialize(result, command2);

  BOOST_TEST(command.mType == command2.mType);
  BOOST_TEST(command.mInstruction.mType == command2.mInstruction.mType);
  BOOST_TEST(command.mInstruction.mOperands[0].mType ==
             command2.mInstruction.mOperands[0].mType);
  BOOST_TEST(command.mInstruction.mOperands[1].mType ==
             command2.mInstruction.mOperands[1].mType);
}

BOOST_AUTO_TEST_SUITE_END();