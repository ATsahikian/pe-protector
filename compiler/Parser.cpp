#include "Parser.h"

#include "common/SCommand.h"
#include "common/Types.h"

#include <boost/fusion/include/adapted.hpp>
#include "boost/tuple/tuple.hpp"
#include "boost/variant.hpp"

#include <assert.h>
#include <ctype.h>
#include <array>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include <range/v3/all.hpp>

#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array[0])))

namespace NPeProtector {
namespace {

// TODO move to parser
int getAttributes(const std::string& attributes) {
  int result = 0;
  for (unsigned int i = 0; i < attributes.size(); ++i) {
    switch (attributes[i]) {
      case 'r':
      case 'R':
        result |= NSectionAttributes::READ;
        break;
      case 'w':
      case 'W':
        result |= NSectionAttributes::WRITE;
        break;
      case 'e':
      case 'E':
        result |= NSectionAttributes::EXECUTE;
        break;
      case 'c':
      case 'C':
        result |= NSectionAttributes::CODE;
        break;
      case 'i':
      case 'I':
        result |= NSectionAttributes::INITIALIZED;
        break;
    }
  }
  return result;
}

NOperand::EType operandType(const NRegister::EType reg) {
  switch (reg) {
    case NRegister::EAX:
    case NRegister::EBX:
    case NRegister::ECX:
    case NRegister::EDX:
    case NRegister::EBP:
    case NRegister::ESP:
    case NRegister::EDI:
    case NRegister::ESI:
      return NOperand::REG32;
    case NRegister::AX:
    case NRegister::BX:
    case NRegister::CX:
    case NRegister::DX:
    case NRegister::BP:
    case NRegister::SP:
    case NRegister::DI:
    case NRegister::SI:
      return NOperand::REG16;
    case NRegister::AL:
    case NRegister::BL:
    case NRegister::CL:
    case NRegister::DL:
    case NRegister::AH:
    case NRegister::BH:
    case NRegister::CH:
    case NRegister::DH:
      return NOperand::REG8;
  }
  throw std::exception();
}
}  // namespace
std::vector<SCommand> parse(std::istream& input) {
  using namespace boost::spirit;
  // call on stream TODO
  std::vector<SCommand> result;
  // return result;

  std::unordered_map<std::string, std::size_t> label2Index;
  std::vector<std::string> nameList;

  input >> std::noskipws;
  auto begin = boost::spirit::istream_iterator{input};
  auto end = boost::spirit::istream_iterator{};

  auto anyChar = x3::rule<class anyChar, char>() = x3::char_ - x3::eol;

  auto quotedString = x3::rule<class quotedString, std::string>() =
      x3::lexeme['"' >> *(x3::char_ - '"') >> '"'];

  auto comment = x3::rule<class comment, std::string>() =
      x3::lit(';') >> *anyChar;

  auto nameFirstChar = x3::rule<class nameFirstChar, char>() =
      x3::alpha | x3::char_('_');
  auto name = x3::rule<class name, std::string>() =
      x3::lexeme[nameFirstChar >>
                 *(nameFirstChar | x3::digit | x3::char_('.'))];

  auto directive = x3::rule<class directive, std::string>() =
      x3::no_case[x3::lit("DIRECTIVE")] >> name;

  auto directiveFn = [&result](const auto& ctx) {
    SCommand d;
    d.mType = NCommand::DIRECTIVE;
    d.mDirective.mName = x3::_attr(ctx);
    result.push_back(d);
  };

  auto import = x3::rule<class import, std::string>() =
      x3::no_case[x3::lit("IMPORT")] >> name;

  auto importFn = [&result, &label2Index](const auto& ctx) {
    // TODO rewrite this code
    const std::string functionName = x3::_attr(ctx);
    SCommand i;
    i.mType = NCommand::IMPORT;
    i.mNameLabel = functionName;
    size_t dot = i.mNameLabel.find('.');
    if (dot != std::string::npos) {
      i.mImport.mDllName = functionName.substr(0, dot) + ".dll";
      i.mImport.mFunctionName = functionName.substr(dot + 1);
    } else {
      throw std::runtime_error{"Wrong import name"};
    }
    label2Index[functionName] = result.size();
    result.push_back(i);
  };

  x3::symbols<uint32_t> dataSizeSymbols;
  // "DD", "DWORD", "DW", "WORD", "DB", "BYTE",
  dataSizeSymbols.add("dd", 4)("dword", 4)("dw", 2)("word", 2)("db", 1)("byte",
                                                                        1);

  x3::symbols<NOperand::EType> memTypeSymbols;
  memTypeSymbols.add("dd", NOperand::MEM32);
  memTypeSymbols.add("dword", NOperand::MEM32);
  memTypeSymbols.add("dw", NOperand::MEM16);
  memTypeSymbols.add("word", NOperand::MEM16);
  memTypeSymbols.add("db", NOperand::MEM8);
  memTypeSymbols.add("byte", NOperand::MEM8);

  auto extern_ = x3::rule<class extern_, std::pair<uint32_t, std::string>>() =
      x3::no_case[x3::lit("EXTERN")] >> x3::no_case[dataSizeSymbols] >> name;

  auto externFn = [&result, &label2Index](auto& ctx) {
    SCommand e;
    e.mType = NCommand::EXTERN;
    e.mNameLabel = x3::_attr(ctx).second;
    // e.mNumberLine = numberLine;
    e.mData.mSizeData = x3::_attr(ctx).first;
    label2Index[e.mNameLabel] = result.size();
    result.push_back(e);
  };

  auto section = x3::rule<
      class sectoin,
      std::pair<std::string /*name*/, std::string /*type TODO: parse it*/>>() =
      x3::no_case[x3::lit("SECTION")] >> quotedString >> name /*type*/;

  auto sectionFn = [&result](auto const& ctx) {
    SCommand s;
    s.mType = NCommand::SECTION;
    // s.mNumberLine = numberLine;
    s.mSection.mName = x3::_attr(ctx).first;
    s.mSection.mAttributes = getAttributes(x3::_attr(ctx).second);
    result.push_back(s);
  };

  auto label = x3::rule<class label, std::string>() = name >> x3::lit(':');
  auto labelFn = [&result, &label2Index](auto const& ctx) {
    label2Index[x3::_attr(ctx)] = result.size();
  };

  x3::symbols<NInstruction::EType> instSymbols;
  // TODO: loop detection
  for (uint32_t i = 0; i < NInstruction::gSize; ++i) {
    instSymbols.add(NInstruction::gStrings[i], NInstruction::EType(i));
  }

  x3::symbols<NRegister::EType> registerSymbols;
  // TODO: loop detection
  for (uint32_t i = 0; i < NRegister::gSize; ++i) {
    registerSymbols.add(NRegister::gStrings[i], NRegister::EType(i));
  }

  x3::symbols<NPrefix::EType> prefixSymbols;
  for (uint32_t i = 0; i < NPrefix::gSize; ++i) {
    prefixSymbols.add(NPrefix::gStrings[i], NPrefix::EType(i));
  }

  x3::symbols<NSegment::EType> segmentSymbols;
  for (uint32_t i = 0; i < NSegment::gSize; ++i) {
    segmentSymbols.add(NSegment::gStrings[i], NSegment::EType(i));
  }

  x3::symbols<NSign::EType> signSymbols;
  signSymbols.add("-", NSign::MINUS)("+", NSign::PLUS);

  NSign::EType signVar{};
  auto signVarSet = [&signVar](auto const& ctx) { signVar = x3::_attr(ctx); };
  auto signVarReset = [&signVar](auto const& ctx) { signVar = NSign::PLUS; };

  uint32_t numVar{};
  auto numVarReset = [&numVar](auto const& ctx) { numVar = 0; };
  auto numVarSet = [&numVar](auto const& ctx) { numVar = x3::_attr(ctx); };
  // TODO check parenthesis
  auto number = x3::rule<class number>() =
      x3::eps[numVarReset] >>
      ((x3::hex >> (x3::lit('h') | x3::lit('H')))[numVarSet] |
       x3::uint_[numVarSet]);

  SConstant constantVar{};
  auto constantVarReset = [&constantVar](auto const&) {
    constantVar = SConstant{};
  };
  auto constantVarNameSet = [&constantVar, &signVar,
                             &nameList](auto const& ctx) {
    // TODO fix this
    constantVar.mLabels.push_back({signVar, nameList.size()});
    nameList.push_back(x3::_attr(ctx));
  };
  auto constantVarNumSet = [&constantVar, &signVar, &numVar](auto const&) {
    constantVar.mValue += signVar == NSign::PLUS ? numVar : -numVar;
  };

  auto nameOrNumber = x3::rule<class nameOrNumber>() =
      name[constantVarNameSet] | number[constantVarNumSet];

  auto constant = x3::rule<class constant>() =
      x3::eps[signVarReset] >> x3::eps[constantVarReset] >>
      -signSymbols[signVarSet] >> nameOrNumber >>
      *(signSymbols[signVarSet] >> nameOrNumber);

  auto data = x3::rule<class data, std::pair<std::string, uint32_t>>() =
      name >> x3::no_case[dataSizeSymbols] >> x3::omit[constant];

  auto dataFn = [&result, &constantVar, &label2Index](auto const& ctx) {
    SCommand d;
    d.mType = NCommand::DATA;
    d.mData.mName = x3::_attr(ctx).first;
    d.mData.mSizeData = x3::_attr(ctx).second;
    d.mData.mConstants.push_back(constantVar);
    d.mData.mCount = 1;  // TODO check
    label2Index[x3::_attr(ctx).first] = result.size();
    result.push_back(d);
  };

  SOperand operandVar = {};
  auto operandVarReset = [&operandVar](auto const&) {
    operandVar = SOperand{};
  };

  auto operandVarMemTypeSet = [&operandVar](auto const& ctx) {
    operandVar.mType = x3::_attr(ctx);
  };

  auto operandVarMemSegmentSet = [&operandVar](auto const& ctx) {
    operandVar.mMemory.mSegment = x3::_attr(ctx);
  };

  auto operandVarMemRegisterSet = [&operandVar](auto const& ctx) {
    operandVar.mMemory.mRegisters.push_back(x3::_attr(ctx));
  };
  auto operandVarMemScaleSet = [&operandVar](auto const& ctx) {
    operandVar.mMemory.mScale = x3::_attr(ctx) - '0';
  };
  auto operandVarMemConstantSet = [&operandVar, &constantVar](auto const&) {
    operandVar.mMemory.mConstant = constantVar;
  };
  auto constantAppend = x3::rule<class constantAppend>() =
      x3::eps[signVarReset] >> x3::eps[constantVarReset] >>
      signSymbols[signVarSet] >> nameOrNumber >>
      *(signSymbols[signVarSet] >> nameOrNumber);

  auto memory = x3::rule<class memory>() =
      (x3::no_case[memTypeSymbols] >>
       x3::no_case["PTR"])[operandVarMemTypeSet] >>
      -(segmentSymbols >> x3::lit(':'))[operandVarMemSegmentSet] >>
      x3::lit('[') >>
      ((registerSymbols[operandVarMemRegisterSet] >>
        -(x3::lit('+') >> registerSymbols)[operandVarMemRegisterSet] >>
        -(x3::lit('*') >> x3::char_("248"))[operandVarMemScaleSet] >>
        -constantAppend[operandVarMemConstantSet]) |
       constant[operandVarMemConstantSet]) >>
      x3::lit(']');

  auto operandVarRegisterSet = [&operandVar](auto const& ctx) {
    operandVar.mRegister = x3::_attr(ctx);
    operandVar.mType = operandType(x3::_attr(ctx));
  };

  auto operandVarConstantSet = [&operandVar, &constantVar](auto const& ctx) {
    operandVar.mConstant = constantVar;
    operandVar.mType = NOperand::CONSTANT;
  };

  auto operand = x3::rule<class operand>() =
      x3::eps[operandVarReset] >>
      (x3::no_case[registerSymbols][operandVarRegisterSet] | memory |
       constant[operandVarConstantSet]);

  SInstruction instVar{};
  auto instVarReset = [&instVar](auto const&) { instVar = SInstruction{}; };
  auto instVarPrefixSet = [&instVar](auto const& ctx) {
    instVar.mPrefix = x3::_attr(ctx);
  };
  auto instVarInstSet = [&instVar](auto const& ctx) {
    instVar.mType = x3::_attr(ctx);
  };
  auto instVarOperandSet = [&instVar, &operandVar](auto const& ctx) {
    instVar.mOperands.push_back(operandVar);
  };

  auto command = x3::rule<class command>() =
      x3::eps[instVarReset] >> -prefixSymbols[instVarPrefixSet] >>
      x3::no_case[instSymbols][instVarInstSet] >>
      -((operand)[instVarOperandSet] % x3::char_(','));

  auto commandFn = [&result, &instVar](const auto& ctx) {
    SCommand i;
    i.mType = NCommand::INSTRUCTION;
    i.mInstruction = instVar;
    result.push_back(i);
  };

  auto const parseResult = x3::phrase_parse(
      begin, end,
      (-(extern_[externFn] | import[importFn] | directive[directiveFn] |
         section[sectionFn] | label[labelFn] |
         (-label[labelFn] >> command[commandFn]) | data[dataFn]) >>
       -comment) %
          x3::eol,
      x3::lit(' ') | x3::lit('\t'));

  if (!parseResult || begin != end) {
    throw std::runtime_error{"Failed to parser asm file"};
  }

  for (auto& r : result) {
    switch (r.mType) {
      case NCommand::DATA:
        for (auto& c : r.mData.mConstants) {
          for (auto& l : c.mLabels) {
            assert(l.mIndex < nameList.size());
            l.mIndex = label2Index.at(nameList[l.mIndex]);
          }
        }
        break;
      case NCommand::INSTRUCTION:
        for (auto& o : r.mInstruction.mOperands) {
          for (auto& l : o.mConstant.mLabels) {
            assert(l.mIndex < nameList.size());
            l.mIndex = label2Index.at(nameList[l.mIndex]);
          }
          for (auto& l : o.mMemory.mConstant.mLabels) {
            assert(l.mIndex < nameList.size());
            l.mIndex = label2Index.at(nameList[l.mIndex]);
          }
        }
        break;
    }
  }

  SCommand e = {};
  e.mType = NCommand::END;
  result.push_back(e);

  // TODO
  loggingCommands(result);

  return result;
}

}  // namespace NPeProtector