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

  std::vector<SCommand> result;
  std::unordered_map<std::string, std::size_t> label2Index;
  std::vector<std::string> nameList;

  input >> std::noskipws;
  auto begin = boost::spirit::istream_iterator{input};
  auto end = boost::spirit::istream_iterator{};

  x3::symbols<x3::unused_type> _keywords;
  _keywords.add("DD", x3::unused);
  _keywords.add("DWORD", x3::unused);
  _keywords.add("DW", x3::unused);
  _keywords.add("WORD", x3::unused);
  _keywords.add("DB", x3::unused);
  _keywords.add("BYTE", x3::unused);
  _keywords.add("EXTERN", x3::unused);
  _keywords.add("SECTION", x3::unused);
  _keywords.add("PTR", x3::unused);
  _keywords.add("DUP", x3::unused);
  _keywords.add("IMPORT", x3::unused);
  _keywords.add("DIRECTIVE", x3::unused);

  x3::symbols<uint32_t> dataSizeSymbols;
  // "DD", "DWORD", "DW", "WORD", "DB", "BYTE",
  dataSizeSymbols.add("dd", 4);
  dataSizeSymbols.add("dword", 4);
  dataSizeSymbols.add("dw", 2);
  dataSizeSymbols.add("word", 2);
  dataSizeSymbols.add("db", 1);
  dataSizeSymbols.add("byte", 1);

  x3::symbols<NOperand::EType> memTypeSymbols;
  memTypeSymbols.add("dd", NOperand::MEM32);
  memTypeSymbols.add("dword", NOperand::MEM32);
  memTypeSymbols.add("dw", NOperand::MEM16);
  memTypeSymbols.add("word", NOperand::MEM16);
  memTypeSymbols.add("db", NOperand::MEM8);
  memTypeSymbols.add("byte", NOperand::MEM8);

  x3::symbols<NInstruction::EType> instSymbols;
  // TODO: loop detection
  for (uint32_t i = 0; i < NInstruction::gSize; ++i) {
    instSymbols.add(NInstruction::gStrings[i], NInstruction::EType(i));
    _keywords.add(NInstruction::gStrings[i], x3::unused);
  }

  x3::symbols<NRegister::EType> registerSymbols;
  // TODO: loop detection
  for (uint32_t i = 0; i < NRegister::gSize; ++i) {
    registerSymbols.add(NRegister::gStrings[i], NRegister::EType(i));
    _keywords.add(NRegister::gStrings[i], x3::unused);
  }

  x3::symbols<NPrefix::EType> prefixSymbols;
  for (uint32_t i = 0; i < NPrefix::gSize; ++i) {
    prefixSymbols.add(NPrefix::gStrings[i], NPrefix::EType(i));
    _keywords.add(NPrefix::gStrings[i], x3::unused);
  }

  x3::symbols<NSegment::EType> segmentSymbols;
  for (uint32_t i = 0; i < NSegment::gSize; ++i) {
    segmentSymbols.add(NSegment::gStrings[i], NSegment::EType(i));
    _keywords.add(NSegment::gStrings[i], x3::unused);
  }

  x3::symbols<NSign::EType> signSymbols;
  signSymbols.add("-", NSign::MINUS)("+", NSign::PLUS);

  auto const keywords =
      x3::lexeme[x3::no_case[_keywords] >> !(x3::alnum | '_')];

  auto blank = x3::omit[+x3::blank];

  auto anyChar = x3::rule<class anyChar, char>() = x3::char_ - x3::eol;

  auto quotedString = x3::rule<class quotedString, std::string>() =
      x3::lexeme['"' >> *(x3::char_ - '"') >> '"'];

  auto comment = x3::rule<class comment>() = x3::lit(';') >> x3::omit[*anyChar];

  auto name = x3::rule<class name, std::string>() =
      x3::lexeme[(x3::alpha | x3::char_('_')) >>
                 *(x3::alnum | x3::char_('.') | x3::char_('_'))] -
      keywords;

  auto directive = x3::rule<class directive, std::string>() =
      x3::lexeme[x3::lit("DIRECTIVE") >> blank >> name];

  // TODO think about top level functions
  auto directiveFn = [&result](const auto& ctx) {
    SCommand d;
    d.mType = NCommand::DIRECTIVE;
    d.mDirective.mName = x3::_attr(ctx);
    result.push_back(d);
  };

  auto import = x3::rule<class import, std::string>() =
      x3::lexeme[x3::no_case[x3::lit("IMPORT")] >> blank >> name];

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

  auto extern_ = x3::rule<class extern_, std::pair<uint32_t, std::string>>() =
      x3::lexeme[x3::no_case[x3::lit("EXTERN")] >> blank >>
                 x3::no_case[dataSizeSymbols] >> blank >> name];

  auto externFn = [&result, &label2Index](auto& ctx) {
    SCommand e;
    e.mType = NCommand::EXTERN;
    e.mNameLabel = x3::_attr(ctx).second;
    e.mData.mSizeData = x3::_attr(ctx).first;
    label2Index[e.mNameLabel] = result.size();
    result.push_back(e);
  };

  auto section = x3::rule<
      class sectoin,
      std::pair<std::string /*name*/, std::string /*type TODO: parse it*/>>() =
      x3::lexeme[x3::no_case[x3::lit("SECTION")] >> blank >> quotedString >>
                 blank >> name /*type*/];

  auto sectionFn = [&result](auto const& ctx) {
    SCommand s;
    s.mType = NCommand::SECTION;
    s.mSection.mName = x3::_attr(ctx).first;
    s.mSection.mAttributes = getAttributes(x3::_attr(ctx).second);
    result.push_back(s);
  };

  auto label = x3::rule<class label, std::string>() = name >> x3::lit(':');
  auto labelFn = [&result, &label2Index](auto const& ctx) {
    label2Index[x3::_attr(ctx)] = result.size();
  };

  NSign::EType signVar{};
  auto signVarSet = [&signVar](auto const& ctx) { signVar = x3::_attr(ctx); };
  auto signVarReset = [&signVar](auto const& ctx) { signVar = NSign::PLUS; };

  auto number = x3::rule<class number, int32_t>() =
      x3::lexeme[(x3::hex >> (x3::lit('h') | x3::lit('H')))] | x3::uint_;

  auto constantVarNameSet = [&signVar, &nameList](auto& ctx) {
    x3::_val(ctx).mLabels.push_back({signVar, nameList.size()});
    nameList.push_back(x3::_attr(ctx));
  };
  auto constantVarNumSet = [&signVar](auto& ctx) {
    x3::_val(ctx).mValue +=
        signVar == NSign::PLUS ? x3::_attr(ctx) : -x3::_attr(ctx);
  };

  auto constant = x3::rule<class constant, SConstant>() =
      x3::eps[signVarReset] >>
      ((name[constantVarNameSet] | number[constantVarNumSet]) %
       signSymbols[signVarSet]);

  auto constantAppend = x3::rule<class constantAppend, SConstant>() =
      x3::eps[signVarReset] >> signSymbols[signVarSet] >>
      ((name[constantVarNameSet] | number[constantVarNumSet]) %
       signSymbols[signVarSet]);

  auto constantStr = x3::rule<class constantStr, std::string>() =
      x3::lexeme['"' >> +(x3::char_ - '"') >> '"'];

  auto dataStr =
      x3::rule<class dataStr, std::pair<std::string, std::string>>() =
          x3::lexeme[name >> blank >>
                     x3::no_case[x3::lit("DB") | x3::lit("BYTE")] >> blank] >>
          constantStr;

  auto dataStrFn = [&result, &label2Index](auto const& ctx) {
    SCommand d;
    d.mType = NCommand::DATA;
    d.mData.mName = x3::_attr(ctx).first;
    d.mData.mSizeData = 1;  // byte size
    const std::string& str = x3::_attr(ctx).second;
    std::transform(str.cbegin(), str.cend(),
                   std::back_inserter(d.mData.mConstants), [](char c) {
                     return SConstant{{}, uint32_t(c)};
                   });
    d.mData.mCount = 1;
    label2Index[x3::_attr(ctx).first] = result.size();
    result.push_back(d);
  };

  auto constantVarListPush = [](auto& ctx) {
    x3::_val(ctx).push_back(x3::_attr(ctx));
  };
  auto constantList = x3::rule<class constantList, std::vector<SConstant>>() =
      constant[constantVarListPush] % x3::lit(',');
  auto data =
      x3::rule<class data,
               boost::tuple<std::string, uint32_t, std::vector<SConstant>>>() =
          name >> x3::no_case[dataSizeSymbols] >> constantList;

  auto dataFn = [&result, &label2Index](auto& ctx) {
    SCommand d;
    d.mType = NCommand::DATA;
    d.mData.mName = boost::get<0>(x3::_attr(ctx));
    d.mData.mSizeData = boost::get<1>(x3::_attr(ctx));
    d.mData.mConstants = boost::get<2>(x3::_attr(ctx));
    d.mData.mCount = 1;
    label2Index[boost::get<0>(x3::_attr(ctx))] = result.size();
    result.push_back(d);
  };

  auto dataDup =
      x3::rule<class dataDup,
               boost::tuple<std::string, uint32_t, int32_t, int32_t>>() =
          x3::lexeme[name >> blank >> x3::no_case[dataSizeSymbols] >> blank >>
                     number >> blank >> x3::no_case[x3::lit("DUP")]] >>
          x3::lit('(') >> number >> x3::lit(')');

  auto dataDupFn = [&result, &label2Index](auto& ctx) {
    SCommand d;
    d.mType = NCommand::DATA;
    d.mData.mName = boost::get<0>(x3::_attr(ctx));
    d.mData.mSizeData = boost::get<1>(x3::_attr(ctx));
    d.mData.mConstants.push_back(
        // TODO remove static_cast
        SConstant{{}, static_cast<uint32_t>(boost::get<3>(x3::_attr(ctx)))});
    d.mData.mCount = boost::get<2>(x3::_attr(ctx));
    label2Index[boost::get<0>(x3::_attr(ctx))] = result.size();
    result.push_back(d);
  };

  auto operandVarMemTypeSet = [](auto& ctx) {
    x3::_val(ctx).mType = x3::_attr(ctx);
  };
  auto operandVarMemSegmentSet = [](auto& ctx) {
    x3::_val(ctx).mMemory.mSegment = x3::_attr(ctx);
  };
  auto operandVarMemRegisterSet = [](auto& ctx) {
    x3::_val(ctx).mMemory.mRegisters.push_back(x3::_attr(ctx));
  };
  auto operandVarMemScaleSet = [](auto& ctx) {
    x3::_val(ctx).mMemory.mScale = x3::_attr(ctx) - '0';
  };
  auto operandVarMemConstantSet = [](auto& ctx) {
    x3::_val(ctx).mMemory.mConstant = x3::_attr(ctx);
  };

  auto memory =
      x3::lexeme[x3::no_case[memTypeSymbols] >> blank >> x3::no_case["PTR"]]
                [operandVarMemTypeSet] >>
      -x3::no_skip[blank >> segmentSymbols >> -blank >> x3::lit(':')]
                  [operandVarMemSegmentSet] >>
      x3::lit('[') >>
      ((registerSymbols[operandVarMemRegisterSet] >>
        -(x3::lit('+') >> registerSymbols)[operandVarMemRegisterSet] >>
        -(x3::lit('*') >> x3::char_("248"))[operandVarMemScaleSet] >>
        -constantAppend[operandVarMemConstantSet]) |
       constant[operandVarMemConstantSet]) >>
      x3::lit(']');

  auto operandVarRegisterSet = [](auto& ctx) {
    x3::_val(ctx).mRegister = x3::_attr(ctx);
    x3::_val(ctx).mType = operandType(x3::_attr(ctx));
  };

  auto operandVarConstantSet = [](auto& ctx) {
    x3::_val(ctx).mConstant = x3::_attr(ctx);
    x3::_val(ctx).mType = NOperand::CONSTANT;
  };

  auto operand = x3::rule<class operand, SOperand>() =
      x3::skip[x3::no_case[registerSymbols][operandVarRegisterSet] | memory |
               constant[operandVarConstantSet]];

  auto instVarPrefixSet = [](auto& ctx) {
    x3::_val(ctx).mPrefix = x3::_attr(ctx);
  };
  auto instVarInstSet = [](auto& ctx) { x3::_val(ctx).mType = x3::_attr(ctx); };
  auto instVarOperandSet = [](auto& ctx) {
    x3::_val(ctx).mOperands.push_back(x3::_attr(ctx));
  };

  auto inst = x3::rule<class inst, SInstruction>() =
      x3::lexeme[-(prefixSymbols >> blank)[instVarPrefixSet] >>
                 (x3::no_case[instSymbols] >>
                  !x3::char_("a-zA-Z0-9"))[instVarInstSet]] >>
      -(operand[instVarOperandSet] % x3::char_(','));

  auto instFn = [&result](const auto& ctx) {
    SCommand i;
    i.mType = NCommand::INSTRUCTION;
    i.mInstruction = x3::_attr(ctx);
    result.push_back(i);
  };

  auto const parseResult = x3::phrase_parse(
      begin, end,
      (-(extern_[externFn] | import[importFn] | directive[directiveFn] |
         section[sectionFn] | (-label[labelFn] >> inst[instFn]) |
         label[labelFn] | dataStr[dataStrFn] | dataDup[dataDupFn] |
         data[dataFn]) >>
       -comment) %
          x3::eol,
      x3::blank);

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

  result.push_back({NCommand::END});

  // TODO debug
  // loggingCommands(result);

  return result;
}

}  // namespace NPeProtector