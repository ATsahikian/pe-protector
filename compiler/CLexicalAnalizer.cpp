#include "CLexicalAnalizer.h"

#include "common\SCommand.h"

#include "common\Types.h"

#include "boost/variant.hpp"
//#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/adapted.hpp>
//#include <boost/fusion/include/std_pair.hpp>
#include "boost/tuple/tuple.hpp"

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

constexpr std::array tokenCategories = {",",
                                        //      ".",
                                        ":", "-", "+", "(", ")", "[", "]", "*",
                                        // keywords
                                        "IMPORT", "EXTERN", "DUP", "PTR",
                                        "SECTION", "DIRECTIVE"};

std::vector<std::string> splitLine(std::string line) {
  std::vector<std::string> result;

  line = line | ranges::views::take_while([](auto c) { return c != ';'; }) |
         ranges::to<std::string>();

  for (unsigned int beginPosition = 0, length = 0;
       beginPosition < line.size();) {
    if ((beginPosition + length == line.size()) ||
        line[beginPosition + length] == ' ' ||
        line[beginPosition + length] == '\t') {
      if (length > 0) {
        // add name
        result.push_back(line.substr(beginPosition, length));
      }
      beginPosition += length + 1;
      length = 0;
    } else if (line[beginPosition + length] == '"') {
      if (length > 0) {
        // add name
        result.push_back(line.substr(beginPosition, length));
      }

      const size_t beginStringPosition = beginPosition + length;

      // add string
      const size_t quotePosition = line.find("\"", beginStringPosition + 1);
      if (quotePosition == std::string::npos) {
        throw std::runtime_error("missing double quote");
      } else {
        const std::size_t endStringPosition = quotePosition + 1;
        // TODO do we need copy quotes here
        result.push_back(line.substr(beginStringPosition,
                                     endStringPosition - beginStringPosition));
      }
      beginPosition = quotePosition + 1;
      length = 0;
    } else if (!isalnum(line[beginPosition + length]) &&
               line[beginPosition + length] != '_' &&
               line[beginPosition + length] != '.') {
      if (length > 0) {
        // add name
        result.push_back(line.substr(beginPosition, length));
      }
      // add character
      result.push_back(line.substr(beginPosition + length, 1));

      beginPosition += length + 1;
      length = 0;
    } else {
      length += 1;
    }
  }
  return result;
}

#if 0
std::vector<std::string> splitLines(std::istream& input) {
  constexpr static auto source = {0, 1, 0, 2, 3, 0, 4, 5, 6, 0, 7, 8, 9};
  constexpr int delimiter{0};
  constexpr ranges::split_view split_view{source, delimiter};

  auto a = ranges::to<std::vector<std::string>>();

  auto const ints = {0, 1, 2, 3, 4, 5};
  auto even = [](int i) { return 0 == i % 2; };
  auto square = [](int i) { return i * i; };

  // "pipe" syntax of composing the views:
  for (int i :
       ints | ranges::views::filter(even) | ranges::views::transform(square)) {
    std::cout << i << ' ';
  }

  std::cout << '\n';

  // a traditional "functional" composing syntax:
  for (int i :
       ranges::views::transform(ranges::views::filter(ints, even), square)) {
    std::cout << i << ' ';
  }

  std::string s = "123 345 678 ";
  std::stringstream ss{s};

  auto splitText =
      ranges::getlines(input) | ranges::to<std::vector<std::string>>();
  /*s | ranges::view::split(' ') | ranges::to<std::vector<std::string>>();*/

  auto splitText3 = ranges::istream_view<std::string>{ss} |
                    ranges::to<std::vector<std::string>>();

  /*auto splitText2 = ranges::istream_view<char>{ss} | ranges::view::split(' ')
     | ranges::to<std::vector<std::string>>();*/

  /*  auto splitText = ranges::istream_view<char>{input} |
                     ranges::view::split(' ') |
                     ranges::to<std::vector<std::string>>();*/

  for (auto s : splitText) {
    std::cout << s << std::endl;
  }
  return {};
  /*
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
      lines.push_back(line);
    }
    return lines;*/
}
#endif

std::vector<std::vector<std::string>> split(std::istream& input) {
  std::vector<std::vector<std::string>> tokens;
  for (const auto& line : ranges::getlines(input)) {
    tokens.push_back(splitLine(line));
  }
  return tokens;
}

// TODO find count of vector
SToken getToken(const std::string& stringToken) {
  assert(!stringToken.empty());

  // scan for standard tokens
  for (int i = 0; i < tokenCategories.size(); ++i) {
    if (!_strcmpi(stringToken.c_str(), tokenCategories[i])) {
      return SToken{NCategory::EType(i)};
    }
  }

  // TODO: create loop for this

  // scan for instructions
  for (int i = 0; i < NInstruction::gSize; ++i) {
    if (!_strcmpi(stringToken.c_str(), NInstruction::gStrings[i])) {
      return SToken{NCategory::INSTRUCTION, i};
    }
  }

  // scan for registers
  for (int i = 0; i < NRegister::gSize; ++i) {
    if (!_strcmpi(stringToken.c_str(), NRegister::gStrings[i])) {
      return SToken{NCategory::REGISTER, i};
    }
  }

  // scan for prefixes
  for (int i = 0; i < NPrefix::gSize; ++i) {
    if (!_strcmpi(stringToken.c_str(), NPrefix::gStrings[i])) {
      return SToken{NCategory::PREFIX, i};
    }
  }

  // scan for data types
  for (int i = 0; i < NDataType::gSize; ++i) {
    if (!_strcmpi(stringToken.c_str(), NDataType::gStrings[i])) {
      return SToken{NCategory::DATA_TYPE, i};
    }
  }

  // scan for segments
  for (int i = 0; i < NSegment::gSize; ++i) {
    if (!_strcmpi(stringToken.c_str(), NSegment::gStrings[i])) {
      return SToken{NCategory::SEGMENT, i};
    }
  }

  if (stringToken[0] == '"') {
    return SToken{NCategory::STRING, 0,
                  stringToken.substr(1, stringToken.size() - 2)};
  }

  if (isdigit(stringToken[0])) {
    int base = 10;
    std::string digits = stringToken;

    if (stringToken.back() == 'h' || stringToken.back() == 'H') {
      base = 16;
      digits = stringToken.substr(0, stringToken.size() - 1);
    }

    size_t index = 0;
    const unsigned int constant = stoul(digits, &index, base);
    if (index != digits.size()) {
      throw std::runtime_error(("wrong number : " + stringToken).c_str());
    }
    return SToken{NCategory::CONSTANT, 0, "", constant};
  }

  return SToken{NCategory::NAME, 0, stringToken};
}
}  // namespace

namespace NCategory {
const char* const gStrings[] = {
    "COMMA",  // ','
    //"DOT",           // '.' for import
    "COLON",          // ':'
    "MINUS",          // '-'
    "PLUS",           // '+'
    "OP_BRACKET",     // '('
    "CL_BRACKET",     // ')'
    "OP_SQ_BRACKET",  // '['
    "CL_SQ_BRACKET",  // ']'
    "ASTERIX",        // '*'
    // keywords
    "IMPORT", "EXTERN", "DUP", "PTR", "SECTION", "DIRECTIVE",
    // groups
    "DATA_TYPE",    // map to type NDataType::EType
    "PREFIX",       // map to type NPrefix::EType
    "INSTRUCTION",  // map to type NInstruction::EType
    "REGISTER",     // map to type NRegister::EType
    "SEGMENT",      // map to type NSegment::EType
    // undefined string
    "NAME", "STRING", "CONSTANT"};
const int gSize = ARRAY_SIZE(gStrings);
}  // namespace NCategory

bool isMatch(const std::vector<SToken>& tokens,
             const std::vector<NCategory::EType>& categories) {
  bool result = categories.empty();
  if (!result) {
    unsigned int i = 0;
    for (; i < categories.size() && i < tokens.size(); ++i) {
      if (tokens[i].mType != categories[i]) {
        break;
      }
    }
    result = i == categories.size();
  }
  return result;
}

std::vector<std::vector<SToken>> parse(std::istream& input) {
  using namespace boost::spirit;
  // call on stream TODO
  std::vector<std::vector<SToken>> result;

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

  auto import = x3::rule<class import, std::string>() =
      x3::no_case[x3::lit("IMPORT")] >> name;

  auto importFn = [&result](const auto& ctx) {
    const std::vector<SToken> ts{SToken{NCategory::IMPORT},
                                 SToken{NCategory::NAME, 0, x3::_attr(ctx)}};
    assert(!result.empty());
    assert(result.back().empty());
    std::copy(ts.cbegin(), ts.cend(), std::back_inserter(result.back()));
  };

  // TODO: Fix to real size
  x3::symbols<uint32_t> dataSymbols;
  // "DD", "DWORD", "DW", "WORD", "DB", "BYTE",
  dataSymbols.add("dd", 0)("dword", 1)("dw", 2)("word", 3)("db", 4)("byte", 5);

  x3::symbols<NOperand::EType> memTypeSymbols;
  dataSymbols.add("dd", NOperand::MEM32);
  dataSymbols.add("dword", NOperand::MEM32);
  dataSymbols.add("dw", NOperand::MEM16);
  dataSymbols.add("word", NOperand::MEM16);
  dataSymbols.add("db", NOperand::MEM8);
  dataSymbols.add("byte", NOperand::MEM8);

  auto extern_ =
      x3::rule<class data, std::pair<int32_t /*index TODO change to size*/,
                                     std::string>>() =
          x3::no_case[x3::lit("EXTERN")] >> x3::no_case[dataSymbols] >> name;

  auto externFn = [&result](auto& ctx) {
    const std::vector<SToken> ts{
        SToken{NCategory::EXTERN},
        SToken{NCategory::DATA_TYPE, x3::_attr(ctx).first},
        SToken{NCategory::NAME, 0, x3::_attr(ctx).second}};

    assert(!result.empty());
    assert(result.back().empty());
    std::copy(ts.cbegin(), ts.cend(), std::back_inserter(result.back()));
  };

  auto section = x3::rule<
      class sectoin,
      std::pair<std::string /*name*/, std::string /*type TODO: parse it*/>>() =
      x3::no_case[x3::lit("SECTION")] >> quotedString >> name /*type*/;

  auto sectionFn = [&result](auto const& ctx) {
    const std::vector<SToken> ts{
        SToken{NCategory::SECTION},
        SToken{NCategory::STRING, 0, x3::_attr(ctx).first},
        SToken{NCategory::NAME, 0, x3::_attr(ctx).second}};
    assert(!result.empty());
    assert(result.back().empty());
    std::copy(ts.cbegin(), ts.cend(), std::back_inserter(result.back()));
  };

  auto label = x3::rule<class label, std::string>() = name >> x3::char_(':');
  auto labelFn = [&result](auto const& ctx) {
    const std::vector<SToken> ts{SToken{NCategory::NAME, 0, x3::_attr(ctx)},
                                 SToken{NCategory::COLON}};
    assert(!result.empty());
    std::copy(ts.cbegin(), ts.cend(), std::back_inserter(result.back()));
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
  auto numVarSetDec = [&numVar](auto const& ctx) { numVar = x3::_attr(ctx); };
  auto numVarSetHex = [&numVar](auto const& ctx) {
    size_t index = 0;
    numVar = std::stoul(x3::_attr(ctx), &index, 16);
    if (index != x3::_attr(ctx).size()) {
      // TODO check
      throw std::exception();
    }
  };
  // TODO check parenthesis
  auto number = x3::rule<class number>() =
      x3::eps[numVarReset] >> x3::uint_[numVarSetDec] |
      // TODO x3::hex check
      (+x3::char_("0-9a-fA-F") >> (x3::lit('h') | x3::lit('H')))[numVarSetHex];

  SConstant constantVar{};
  auto constantVarReset = [&constantVar](auto const&) {
    constantVar = SConstant{};
  };
  auto constantVarNameSet = [&constantVar, &signVar](auto const&) {
    // TODO fix this
    constantVar.mLabels.push_back({signVar, 0 /*x3::attr_(ctx)*/});
  };
  auto constantVarNumSet = [&constantVar, signVar, numVar](auto const&) {
    constantVar.mValue += signVar == NSign::PLUS ? numVar : -numVar;
  };

  auto constant = x3::rule<class constant>() =
      x3::eps[signVarReset] >> x3::eps[constantVarReset] >>
      -signSymbols[signVarSet] >>
      (name[constantVarNameSet] | number[constantVarNumSet]) >>
      *(signSymbols[signVarSet] >>
        (name[constantVarNameSet] | number[constantVarNumSet]));

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
    operandVar.mMemory.mScale = x3::_attr(ctx);
  };
  auto operandVarMemConstantSet = [&operandVar, constantVar](auto const&) {
    operandVar.mMemory.mConstant = constantVar;
  };
  // DWORD PTR FS:[ebx + ecx*4 + mem_location]
  auto memory = x3::rule<class memory>() =
      (x3::no_case[memTypeSymbols] >>
       x3::no_case["PTR"])[operandVarMemTypeSet] >>
          -(segmentSymbols >> x3::lit(':'))[operandVarMemSegmentSet] >>
          x3::lit('[') >>
          -(registerSymbols[operandVarMemRegisterSet] >>
            -registerSymbols[operandVarMemRegisterSet] >>
            -(x3::lit('*') >>
              /*TODO: 2,4,8:symbols*/ x3::int_)[operandVarMemScaleSet] >>
            -(x3::lit('+') >> constant)[operandVarMemConstantSet]) |
      constant[operandVarMemConstantSet] >> x3::lit(']');

  auto operandVarRegisterSet = [&operandVar](auto const& ctx) {
    operandVar.mRegister = x3::_attr(ctx);
    operandVar.mType = operandType(x3::_attr(ctx));
  };

  auto operandVarConstantSet = [&operandVar, constantVar](auto const& ctx) {
    operandVar.mConstant = constantVar;
    operandVar.mType = NOperand::CONSTANT;
  };

  auto operand = x3::rule<class operand>() =
      x3::eps[operandVarReset] >> (registerSymbols[operandVarRegisterSet] |
                                   memory | constant[operandVarConstantSet]);

  SInstruction instVar;
  auto instVarReset = [&instVar](auto const&) { instVar = SInstruction{}; };
  auto instVarPrefixSet = [&instVar](auto const& ctx) {
    instVar.mPrefix = x3::_attr(ctx);
  };
  auto instVarInstSet = [&instVar](auto const& ctx) {
    instVar.mType = x3::_attr(ctx);
  };
  auto instVarOperandSet = [&instVar, operandVar](auto const& ctx) {
    instVar.mOperands.push_back(operandVar);
  };

  auto command = x3::rule<class command>() =
      x3::eps[instVarReset] >> -prefixSymbols[instVarPrefixSet] >>
      instSymbols[instVarInstSet] >>
      -((operand)[instVarOperandSet] % x3::char_(','));

  auto commandFn = [&result](const auto& ctx) {
    // TODO copy instruction
    /*  const std::vector<SToken> ts{SToken{NCategory::IMPORT},
                                   SToken{NCategory::NAME, 0, x3::_attr(ctx)}};
      assert(!result.empty());
      assert(result.back().empty());
      std::copy(ts.cbegin(), ts.cend(), std::back_inserter(result.back()));*/
  };

  // TODO back_insertert ?
  auto pushLine = [&result]() { result.push_back({}); };

  pushLine();

  auto r = x3::phrase_parse(
      begin, end,
      (-(extern_[externFn] | import[importFn] | section[sectionFn] |
         label[labelFn] | (label[labelFn] >> command[commandFn])) >>
       -comment) %
          x3::eol[pushLine],
      x3::lit(' ') | x3::lit('\t'));

  std::cout << "phrase_parse result " << r << " result " << result.size()
            << std::endl;

  for (auto vt : result) {
    for (auto t : vt) {
      std::cout << "{" << NCategory::gStrings[t.mType] << "," << t.mIndex
                << ",\"" << t.mData << "\"," << t.mConstant << "}";
    }
    std::cout << std::endl;
  }

  return result;

  class my_visitor : public boost::static_visitor<> {
   public:
    void operator()(const std::string& str) const { std::cout << str; }
  };  // namespace NPeProtector

  /*    for (auto i : vs) {
        if (i.first) {
          boost::apply_visitor(my_visitor(), *i.first);

          // std::cout << (*i.first).which();

          // std::cout << *i.first;
        }

        if (i.second) {
          std::cout << *i.second;
        }
        std::cout << std::endl;
      }
  */
  // std::cout << "r " << r << " vs " << vs.size() << std::endl;
  /*std::copy(vs.begin(), vs.end(),
            std::ostream_iterator<std::string>{std::cout, "\n"});*/

  //  auto r = x3::phrase_parse(begin, end,
  //                           +(x3::char_('1') | x3::char_('_')) % x3::eol,
  //                         x3::lit(' '), a);
  /*std::cout << "r " << r << " a " << a.size() << std::endl;

  */
  /*
    const std::vector<std::vector<std::string>>& tokens = split(input);

    std::vector<std::vector<SToken>> result;

    for (unsigned int i = 0; i < tokens.size(); ++i) {
      std::vector<SToken> lineTokens;

      for (unsigned int j = 0; j < tokens[i].size(); ++j) {
        if (!tokens[i][j].empty()) {
          lineTokens.push_back(getToken(tokens[i][j]));
        }
      }
      result.push_back(lineTokens);
    }

    return result;*/
}
}  // namespace NPeProtector