#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Library/SCommand.h"
#include "../PeProtector/Mutation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace NPeProtector;

namespace Test
{
	TEST_CLASS(MutationTest)
	{
	public:
		
      TEST_METHOD(testMutateCommandsMov)
		{
         SOperand operand1;
         operand1.mType = NOperand::REG32;
         operand1.mRegister = NRegister::EAX;

         SOperand operand2;
         operand2.mType = NOperand::REG32;
         operand2.mRegister = NRegister::EBX;

         SInstruction instruction(NPrefix::NON, NInstruction::MOV, { operand1, operand2 });

         SCommand movCommand;
         movCommand.mType = NCommand::INSTRUCTION;
         movCommand.mInstruction = instruction;

         std::vector<SCommand> commands = { movCommand };

         mutateCommands(commands);

         SCommand pushCommand;
         pushCommand.mType = NCommand::INSTRUCTION;
         pushCommand.mInstruction.mType = NInstruction::PUSH;
         pushCommand.mInstruction.mOperands.push_back(operand2);

         SCommand popCommand;
         popCommand.mType = NCommand::INSTRUCTION;
         popCommand.mInstruction.mType = NInstruction::POP;
         popCommand.mInstruction.mOperands.push_back(operand1);

         std::vector<SCommand> expectedResult = { pushCommand , popCommand};
         
         Assert::IsTrue(expectedResult[0].mType == commands[0].mType);
         Assert::IsTrue(expectedResult[0].mInstruction.mType == commands[0].mInstruction.mType);
         Assert::IsTrue(expectedResult[0].mInstruction.mOperands[0].mType == commands[0].mInstruction.mOperands[0].mType);
         Assert::IsTrue(expectedResult[0].mInstruction.mOperands[0].mRegister == commands[0].mInstruction.mOperands[0].mRegister);

         Assert::IsTrue(expectedResult[1].mType == commands[1].mType);
         Assert::IsTrue(expectedResult[1].mInstruction.mType == commands[1].mInstruction.mType);
         Assert::IsTrue(expectedResult[1].mInstruction.mOperands[0].mType == commands[1].mInstruction.mOperands[0].mType);
         Assert::IsTrue(expectedResult[1].mInstruction.mOperands[0].mRegister == commands[1].mInstruction.mOperands[0].mRegister);
      }

      TEST_METHOD(testMutateCommandsPush)
      {
         SOperand operand1;
         operand1.mType = NOperand::REG32;
         operand1.mRegister = NRegister::EAX;

         SInstruction instruction(NPrefix::NON, NInstruction::PUSH, { operand1 });

         SCommand pushCommand;
         pushCommand.mType = NCommand::INSTRUCTION;
         pushCommand.mInstruction = instruction;

         std::vector<SCommand> commands = { pushCommand };

         mutateCommands(commands);

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

         SCommand movCommand;
         movCommand.mType = NCommand::INSTRUCTION;
         movCommand.mInstruction.mType = NInstruction::MOV;

         SOperand memOperand;
         memOperand.mType = NOperand::MEM32;
         memOperand.mMemory.mRegisters.push_back(NRegister::ESP);

         movCommand.mInstruction.mOperands.push_back(memOperand);
         movCommand.mInstruction.mOperands.push_back(operand1);

         std::vector<SCommand> expectedResult = { subCommand, movCommand };

         Assert::IsTrue(expectedResult[0].mType == commands[0].mType);
         Assert::IsTrue(expectedResult[0].mInstruction.mType == commands[0].mInstruction.mType);
         Assert::IsTrue(expectedResult[0].mInstruction.mOperands[0].mType == commands[0].mInstruction.mOperands[0].mType);

         Assert::IsTrue(expectedResult[1].mType == commands[1].mType);
         Assert::IsTrue(expectedResult[1].mInstruction.mType == commands[1].mInstruction.mType);
         Assert::IsTrue(expectedResult[1].mInstruction.mOperands[0].mType == commands[1].mInstruction.mOperands[0].mType);
      }
	};
}