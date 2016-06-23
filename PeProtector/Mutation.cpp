#include "Mutation.h"
#include "assert.h"

using std::vector;

namespace NPeProtector
{
namespace
{
   bool isEspInMemory(const SOperand & operand)
   {
      if (operand.mType == NOperand::MEM16 ||
         operand.mType == NOperand::MEM32 ||
         operand.mType == NOperand::MEM8)
      {
         for (unsigned int i = 0; i < operand.mMemory.mRegisters.size(); ++i)
         {
            if (operand.mMemory.mRegisters[i] == NRegister::ESP)
            {
               return true;
            }
         }
      }
      return false;
   }


   vector<SCommand> getPushPop()
   {
      SCommand pushCommand;
      pushCommand.mType = NCommand::INSTRUCTION;
      pushCommand.mInstruction.mType = NInstruction::PUSH;

      SOperand regOperand;
      regOperand.mType = NOperand::REG32;
      regOperand.mRegister = NRegister::EAX;
      pushCommand.mInstruction.mOperands.push_back(regOperand);

      SCommand popCommand;
      popCommand.mType = NCommand::INSTRUCTION;
      popCommand.mInstruction.mType = NInstruction::POP;
      popCommand.mInstruction.mOperands.push_back(regOperand);

      return{ pushCommand, popCommand };
   }

   vector<SCommand> mutateCall(const SCommand & instruction, unsigned int nextCommandPosition)
   {
      assert(instruction.mInstruction.mOperands.size() == 1);
      vector<SCommand> result = { instruction };
      if (instruction.mInstruction.mOperands[0].mType != NOperand::CONSTANT)
      {
         SCommand pushCommand;
         pushCommand.mType = NCommand::INSTRUCTION;
         pushCommand.mNumberLine = instruction.mNumberLine;
         pushCommand.mInstruction.mType = NInstruction::PUSH;

         SOperand constOperand;
         constOperand.mType = NOperand::CONSTANT;
         SLabel label;
         label.mIndex = nextCommandPosition;
         label.mSign = NSign::PLUS;
         constOperand.mConstant.mLabels.push_back(label);

         pushCommand.mInstruction.mOperands.push_back(constOperand);

         SCommand jmpCommand;
         jmpCommand.mType = NCommand::INSTRUCTION;
         jmpCommand.mNumberLine = instruction.mNumberLine;
         jmpCommand.mInstruction.mType = NInstruction::JMP;
         jmpCommand.mInstruction.mOperands.push_back(instruction.mInstruction.mOperands[0]);
         
         result = { pushCommand, jmpCommand };
      }
      return result;
   }

   vector<SCommand> mutatePush(const SCommand & instruction)
   {
      assert(instruction.mInstruction.mOperands.size() == 1);
      
      vector<SCommand> result = { instruction };

      if (instruction.mInstruction.mOperands[0].mType != NOperand::REG8 &&
         !(instruction.mInstruction.mOperands[0].mType == NOperand::REG32 && instruction.mInstruction.mOperands[0].mRegister == NRegister::ESP) &&
         instruction.mInstruction.mOperands[0].mType != NOperand::MEM8 &&
         instruction.mInstruction.mOperands[0].mType != NOperand::MEM16 &&
         instruction.mInstruction.mOperands[0].mType != NOperand::MEM32 &&
         !isEspInMemory(instruction.mInstruction.mOperands[0]))
      {
         SCommand subCommand;
         subCommand.mType = NCommand::INSTRUCTION;
         subCommand.mNumberLine = instruction.mNumberLine;
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
         movCommand.mNumberLine = instruction.mNumberLine;
         movCommand.mInstruction.mType = NInstruction::MOV;
         
         SOperand memOperand;
         memOperand.mType = NOperand::MEM32;
         memOperand.mMemory.mRegisters.push_back(NRegister::ESP);

         movCommand.mInstruction.mOperands.push_back(memOperand);
         movCommand.mInstruction.mOperands.push_back(instruction.mInstruction.mOperands[0]);

         result = { subCommand, movCommand };
      }
      return result;
   }

   vector<SCommand> mutateMov(const SCommand & instruction)
   {
      assert(instruction.mInstruction.mOperands.size() == 2);

      vector<SCommand> result = { instruction };
      if (instruction.mInstruction.mOperands[0].mType != NOperand::REG8 &&
         instruction.mInstruction.mOperands[0].mType != NOperand::MEM8 &&
         instruction.mInstruction.mOperands[1].mType != NOperand::REG8 &&
         instruction.mInstruction.mOperands[1].mType != NOperand::MEM8 &&
         !isEspInMemory(instruction.mInstruction.mOperands[0]))
      {
         SCommand pushCommand;
         pushCommand.mType = NCommand::INSTRUCTION;
         pushCommand.mNumberLine = instruction.mNumberLine;
         pushCommand.mInstruction.mType = NInstruction::PUSH;
         pushCommand.mInstruction.mOperands.push_back(instruction.mInstruction.mOperands[1]);

         SCommand popCommand;
         popCommand.mType = NCommand::INSTRUCTION;
         popCommand.mNumberLine = instruction.mNumberLine;
         popCommand.mInstruction.mType = NInstruction::POP;
         popCommand.mInstruction.mOperands.push_back(instruction.mInstruction.mOperands[0]);

         result = { pushCommand, popCommand };
      }
      return result;
   }

   void shiftConstants(vector<SCommand> & commands, unsigned int position, int shift)
   {
      if (shift > 0)
      {
         for (unsigned int i = 0; i < commands.size(); ++i)
         {
            if (commands[i].mType == NCommand::INSTRUCTION)
            {
               for (unsigned int j = 0; j < commands[i].mInstruction.mOperands.size(); ++j)
               {
                  // shift constant
                  for (unsigned int k = 0; k < commands[i].mInstruction.mOperands[j].mConstant.mLabels.size(); ++k)
                  {
                     if (commands[i].mInstruction.mOperands[j].mConstant.mLabels[k].mIndex > static_cast<int>(position))
                     {
                        commands[i].mInstruction.mOperands[j].mConstant.mLabels[k].mIndex += shift;
                     }
                  }

                  for (unsigned int k = 0; k < commands[i].mInstruction.mOperands[j].mMemory.mConstant.mLabels.size(); ++k)
                  {
                     if (commands[i].mInstruction.mOperands[j].mMemory.mConstant.mLabels[k].mIndex > static_cast<int>(position))
                     {
                        commands[i].mInstruction.mOperands[j].mMemory.mConstant.mLabels[k].mIndex += shift;
                     }
                  }

               }
            }
            else if (commands[i].mType == NCommand::DATA)
            {
               for (unsigned int j = 0; j < commands[i].mData.mConstants.size(); ++j)
               {
                  for (unsigned int k = 0; k < commands[i].mData.mConstants[j].mLabels.size(); ++k)
                  {
                     if (commands[i].mData.mConstants[j].mLabels[k].mIndex > static_cast<int>(position))
                     {
                        commands[i].mData.mConstants[j].mLabels[k].mIndex += shift;
                     }
                  }
               }
            }
         }
      }
   }
}
   void mutateCommands(vector<SCommand> & commands)
   {
      for (unsigned int i = 0; i < commands.size(); ++i)
      {
         if (commands[i].mType == NCommand::INSTRUCTION)
         {
//             switch (commands[i].mInstruction.mType)
//             {
//                case NInstruction::MOV:
//                {
//                   const vector<SCommand> & mutatedCommands = mutateMov(commands[i]);
//                   if (mutatedCommands.size() > 1)
//                   {
//                      commands.erase(commands.begin() + i);
//                      commands.insert(commands.begin() + i, mutatedCommands.begin(), mutatedCommands.end());
// 
//                      shiftConstants(commands, i, mutatedCommands.size() - 1);
//                   }
//                   i += mutatedCommands.size() - 1;
//                   break;
//                }
//                case NInstruction::PUSH:
//                {
//                   const vector<SCommand> & mutatedCommands = mutatePush(commands[i]);
//                   if (mutatedCommands.size() > 1)
//                   {
//                      commands.erase(commands.begin() + i);
//                      commands.insert(commands.begin() + i, mutatedCommands.begin(), mutatedCommands.end());
// 
//                      shiftConstants(commands, i, mutatedCommands.size() - 1);
//                   }
//                   i += mutatedCommands.size() - 1;
//                   break;
//                }
//                case NInstruction::CALL:
//                {
//                   break;
//                }
//             }
            if (i % 5 == 0)
            {
               // insert random instructions!
               const vector<SCommand> & mutatedCommands = getPushPop();
               if (mutatedCommands.size() > 1)
               {
                  commands.insert(commands.begin() + i, mutatedCommands.begin(), mutatedCommands.end());

                  shiftConstants(commands, i, mutatedCommands.size());
               }
               i += 2;
            }
         }
      }
   }
}
