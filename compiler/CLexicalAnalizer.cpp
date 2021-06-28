#include "CLexicalAnalizer.h"

#include "common\Types.h"

#include <assert.h>
#include <ctype.h>
#include <array>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include <range/v3/all.hpp>

#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array[0])))

namespace NPeProtector {
namespace {
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
  // using boost::spirit::x3::parse;
  // using client::print_action;

  // const std::vector<std::vector<std::string>>& tokens = split(input);

  {  // example using function object
/*
    // call on stream
    input >> std::noskipws;

    // std::string i{input.begin(), input.end};

    std::vector<std::vector<char>> a;
    std::string s;

    auto begin = boost::spirit::istream_iterator{input};
    auto end = boost::spirit::istream_iterator{};

    auto anyChar = x3::char_ - x3::eol;
    auto comment = x3::lit(';') >> *anyChar;

    auto nameFirstChar = x3::alpha | x3::char_('_');
    auto name = nameFirstChar >> *(nameFirstChar | x3::digit | x3::char_('.'));

    // all caces
    auto empty = -comment;
    auto import = x3::lit("IMPORT") >> name;
    // auto import2 = x3::lit("IMPORT") >> +anyChar;
    // auto extern_ = x3::lit("EXTERN") >> name;
    // TODO: remove it
    // auto unknown = *anyChar;
*/
    // *(char_ - eol) % eol
    /*    std::string a2;
        auto r2 = x3::phrase_parse(begin, end, +x3::digit, x3::lit(' '), a2);*/

  //  auto r = x3::phrase_parse(begin, end,
   //                           +(x3::char_('1') | x3::char_('_')) % x3::eol,
     //                         x3::lit(' '), a);
    /*std::cout << "r " << r << " a " << a.size() << std::endl;

    std::copy(a.begin(), a.end(),
              std::ostream_iterator<std::string>{std::cout, "\n"});*/
  }
  std::vector<std::vector<SToken>> result;

#if 0


  for (unsigned int i = 0; i < tokens.size(); ++i) {
    std::vector<SToken> lineTokens;

    for (unsigned int j = 0; j < tokens[i].size(); ++j) {
      if (!tokens[i][j].empty()) {
        lineTokens.push_back(getToken(tokens[i][j]));
      }
    }
    result.push_back(lineTokens);
  }
#endif
  return result;
}
}  // namespace NPeProtector