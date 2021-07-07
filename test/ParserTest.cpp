#define BOOST_TEST_MODULE parser test
#include <boost/test/included/unit_test.hpp>

#include "compiler/Parser.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(ParserTest);

BOOST_AUTO_TEST_CASE(testParserComments) {
  std::stringstream input;
  input << ";23\n  ;1\n  ;12";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands.size() == 1);
  BOOST_TEST(commands[0].mType == NCommand::END);
}

BOOST_AUTO_TEST_CASE(testParserImport) {
  std::stringstream input;
  input << "IMPORT KERNEL32.GetTickCount";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::IMPORT);
  BOOST_TEST(commands[0].mImport.mDllName == "KERNEL32.dll");
  BOOST_TEST(commands[0].mImport.mFunctionName == "GetTickCount");
}

BOOST_AUTO_TEST_CASE(testParserData1) {
  std::stringstream input;
  input << "name DD 123";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::DATA);
  BOOST_TEST(commands[0].mData.mName == "name");
  BOOST_TEST(commands[0].mData.mCount == 1);
  BOOST_TEST(commands[0].mData.mConstants[0].mValue == 123);
  BOOST_TEST(commands[0].mData.mSizeData == 4);
}

BOOST_AUTO_TEST_CASE(testParserData2) {
  std::stringstream input;
  input << "name DWORD 123";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::DATA);
  BOOST_TEST(commands[0].mData.mName == "name");
  BOOST_TEST(commands[0].mData.mCount == 1);
  BOOST_TEST(commands[0].mData.mConstants[0].mValue == 123);
  BOOST_TEST(commands[0].mData.mSizeData == 4);
}

BOOST_AUTO_TEST_CASE(testParserDataString) {
  std::stringstream input;
  input << "szLoadLibrary DB \"LoadLibraryA\"";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::DATA);
  BOOST_TEST(commands[0].mData.mName == "szLoadLibrary");
  BOOST_TEST(commands[0].mData.mCount == 1);
  BOOST_TEST(commands[0].mData.mConstants.size() == 12);
  BOOST_TEST(commands[0].mData.mConstants[0].mValue == (DWORD)'L');
  BOOST_TEST(commands[0].mData.mConstants[1].mValue == (DWORD)'o');
  BOOST_TEST(commands[0].mData.mConstants[2].mValue == (DWORD)'a');
  BOOST_TEST(commands[0].mData.mConstants[3].mValue == (DWORD)'d');
  BOOST_TEST(commands[0].mData.mSizeData == 1);
}

BOOST_AUTO_TEST_CASE(testParserDataCount) {
  std::stringstream input;
  input << "name DB 1, 2, 10h";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::DATA);
  BOOST_TEST(commands[0].mData.mName == "name");
  BOOST_TEST(commands[0].mData.mCount == 1);
  BOOST_TEST(commands[0].mData.mConstants[0].mValue == 1);
  BOOST_TEST(commands[0].mData.mConstants[1].mValue == 2);
  BOOST_TEST(commands[0].mData.mConstants[2].mValue == 0x10);
  BOOST_TEST(commands[0].mData.mSizeData == 1);
}

BOOST_AUTO_TEST_CASE(testParserDataDup) {
  std::stringstream input;
  input << "testDup db 30 dup (80h)";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::DATA);
  BOOST_TEST(commands[0].mData.mName == "testDup");
  BOOST_TEST(commands[0].mData.mConstants.size() == 1);
  BOOST_TEST(commands[0].mData.mConstants[0].mValue == 0x80);
  BOOST_TEST(commands[0].mData.mSizeData == 1);
  BOOST_TEST(commands[0].mData.mCount == 30);
}
BOOST_AUTO_TEST_CASE(testParserSection) {
  std::stringstream input;
  input << "SECTION \".text\" crwei";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::SECTION);
  BOOST_TEST(commands[0].mSection.mName == ".text");
  BOOST_TEST((commands[0].mSection.mAttributes & NSectionAttributes::CODE) !=
             0);
  BOOST_TEST((commands[0].mSection.mAttributes & NSectionAttributes::READ) !=
             0);
  BOOST_TEST((commands[0].mSection.mAttributes & NSectionAttributes::EXECUTE) !=
             0);
  BOOST_TEST((commands[0].mSection.mAttributes & NSectionAttributes::WRITE) !=
             0);
  BOOST_TEST((commands[0].mSection.mAttributes &
              NSectionAttributes::INITIALIZED) != 0);
}

BOOST_AUTO_TEST_CASE(testParserInstruction) {
  std::stringstream input;
  input << "MOV EAX, DWORD PTR [EAX + 0CH]";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::INSTRUCTION);
  BOOST_TEST(commands[0].mInstruction.mType == NInstruction::MOV);
  BOOST_TEST(commands[0].mInstruction.mOperands.size() == 2);
  BOOST_TEST(commands[0].mInstruction.mOperands[0].mType == NOperand::REG32);
  BOOST_TEST(commands[0].mInstruction.mOperands[0].mRegister == NRegister::EAX);
  BOOST_TEST(commands[0].mInstruction.mOperands[1].mType == NOperand::MEM32);
  BOOST_TEST(commands[0].mInstruction.mOperands[1].mMemory.mRegisters[0] ==
             NRegister::EAX);
  BOOST_TEST(commands[0].mInstruction.mOperands[1].mMemory.mConstant.mValue ==
             0x0C);
}

BOOST_AUTO_TEST_CASE(testParserExtern) {
  std::stringstream input;
  input << "EXTERN DD externImageBase";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::EXTERN);
  BOOST_TEST(commands[0].mNameLabel == "externImageBase");
  BOOST_TEST(commands[0].mData.mSizeData == 4);
}

BOOST_AUTO_TEST_CASE(testParserLabel1) {
  std::stringstream input;
  input << "labelName: NOP";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::INSTRUCTION);
  BOOST_TEST(commands[0].mInstruction.mType == NInstruction::NOP);
}

BOOST_AUTO_TEST_CASE(testParserLabel2) {
  std::stringstream input;
  input << "labelName: \n\n NOP";

  const std::vector<SCommand>& commands = parse(input);

  BOOST_TEST(commands[0].mType == NCommand::INSTRUCTION);
  BOOST_TEST(commands[0].mInstruction.mType == NInstruction::NOP);
}

BOOST_AUTO_TEST_SUITE_END();