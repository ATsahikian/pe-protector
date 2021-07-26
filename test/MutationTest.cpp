#define BOOST_TEST_MODULE mutation test
#include <boost/test/included/unit_test.hpp>

#include "pe-protector/Mutation.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(MutationTest);

// method for testing mutateCommands with Mov instruction
BOOST_AUTO_TEST_CASE(testMutateCommandsMov) {
  // create command for testing "MOV EAX, EBX"
  SOperand operand1;
  operand1.mType = NOperand::REG32;
  operand1.mRegister = NRegister::EAX;

  SOperand operand2;
  operand2.mType = NOperand::REG32;
  operand2.mRegister = NRegister::EBX;

  SInstruction instruction{
      NPrefix::NON, NInstruction::MOV, {operand1, operand2}};

  SCommand movCommand;
  movCommand.mType = NCommand::INSTRUCTION;
  movCommand.mInstruction = instruction;

  std::vector<SCommand> commands = {movCommand};

  // pass command "MOV EAX, EBX"
  mutateCommands(commands);

  // create instruction "PUSH EBX"
  SCommand pushCommand;
  pushCommand.mType = NCommand::INSTRUCTION;
  pushCommand.mInstruction.mType = NInstruction::PUSH;
  pushCommand.mInstruction.mOperands.push_back(operand2);

  // create instruction "POP EAX"
  SCommand popCommand;
  popCommand.mType = NCommand::INSTRUCTION;
  popCommand.mInstruction.mType = NInstruction::POP;
  popCommand.mInstruction.mOperands.push_back(operand1);

  std::vector<SCommand> expectedResult = {pushCommand, popCommand};

  // compare results
  BOOST_TEST(expectedResult[0].mType == commands[0].mType);
  BOOST_TEST(expectedResult[0].mInstruction.mType ==
             commands[0].mInstruction.mType);
  BOOST_TEST(expectedResult[0].mInstruction.mOperands[0].mType ==
             commands[0].mInstruction.mOperands[0].mType);
  BOOST_TEST(expectedResult[0].mInstruction.mOperands[0].mRegister ==
             commands[0].mInstruction.mOperands[0].mRegister);

  BOOST_TEST(expectedResult[1].mType == commands[1].mType);
  BOOST_TEST(expectedResult[1].mInstruction.mType ==
             commands[1].mInstruction.mType);
  BOOST_TEST(expectedResult[1].mInstruction.mOperands[0].mType ==
             commands[1].mInstruction.mOperands[0].mType);
  BOOST_TEST(expectedResult[1].mInstruction.mOperands[0].mRegister ==
             commands[1].mInstruction.mOperands[0].mRegister);
}

// method for testing mutateCommands with Push instruction
BOOST_AUTO_TEST_CASE(testMutateCommandsPush) {
  // create instruction "PUSH EAX"
  SOperand operand1;
  operand1.mType = NOperand::REG32;
  operand1.mRegister = NRegister::EAX;

  SInstruction instruction{NPrefix::NON, NInstruction::PUSH, {operand1}};

  SCommand pushCommand;
  pushCommand.mType = NCommand::INSTRUCTION;
  pushCommand.mInstruction = instruction;

  std::vector<SCommand> commands = {pushCommand};
  // pass instruction "PUSH EAX"
  mutateCommands(commands);

  // create instruction "SUB ESP, 4"
  SCommand subCommand;
  subCommand.mType = NCommand::INSTRUCTION;
  subCommand.mInstruction.mType = NInstruction::SUB;

  SOperand espOperand;
  espOperand.mType = NOperand::REG32;
  espOperand.mRegister = NRegister::ESP;

  subCommand.mInstruction.mOperands.push_back(espOperand);

  SOperand constOperand;
  constOperand.mType = NOperand::CONSTANT;
  constOperand.mConstant.mValue = 4;

  subCommand.mInstruction.mOperands.push_back(constOperand);

  // create instruction "MOV DWORD PTR [ESP], EAX"
  SCommand movCommand;
  movCommand.mType = NCommand::INSTRUCTION;
  movCommand.mInstruction.mType = NInstruction::MOV;

  SOperand memOperand;
  memOperand.mType = NOperand::MEM32;
  memOperand.mMemory.mRegisters.push_back(NRegister::ESP);

  movCommand.mInstruction.mOperands.push_back(memOperand);
  movCommand.mInstruction.mOperands.push_back(operand1);

  std::vector<SCommand> expectedResult = {subCommand, movCommand};

  // compare results
  BOOST_TEST(expectedResult[0].mType == commands[0].mType);
  BOOST_TEST(expectedResult[0].mInstruction.mType ==
             commands[0].mInstruction.mType);
  BOOST_TEST(expectedResult[0].mInstruction.mOperands[0].mType ==
             commands[0].mInstruction.mOperands[0].mType);

  BOOST_TEST(expectedResult[1].mType == commands[1].mType);
  BOOST_TEST(expectedResult[1].mInstruction.mType ==
             commands[1].mInstruction.mType);
  BOOST_TEST(expectedResult[1].mInstruction.mOperands[0].mType ==
             commands[1].mInstruction.mOperands[0].mType);
}
BOOST_AUTO_TEST_SUITE_END();