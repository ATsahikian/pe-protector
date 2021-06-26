#include "CLexicalAnalizer.h"

#include "common\Types.h"

#include <assert.h>
#include <ctype.h>
#include <array>
#include <iostream>
#include <string>

#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array[0])))

namespace NPeProtector {
namespace {
// todo move to impl
constexpr std::array sCategories = {",",
                                    //      ".",
                                    ":", "-", "+", "(", ")", "[", "]", "*",
                                    // keywords
                                    "IMPORT", "EXTERN", "DUP", "PTR", "SECTION",
                                    "DIRECTIVE"};

std::vector<std::string> splitLine(std::string line) {
  std::vector<std::string> result;

  // skip comments
  line = std::string(line, 0, line.find(';'));

  for (unsigned int beginPosition = 0, length = 0;
       beginPosition < line.size();) {
    if ((beginPosition + length == line.size()) ||
        line[beginPosition + length] == ' ' ||
        line[beginPosition + length] == '\t') {
      if (length > 0) {
        // add name
        result.push_back(line.substr(beginPosition, length));
        beginPosition += length + 1;
        length = 0;
      } else {
        // skip space
        beginPosition += 1;
      }
    } else if (line[beginPosition + length] == '"') {
      if (length > 0) {
        // add name
        result.push_back(line.substr(beginPosition, length));
      }

      const size_t beginStringPosition = beginPosition + length;

      // add string
      const size_t quotePosition = line.find("\"", beginStringPosition + 1);
      if (quotePosition == std::string::npos) {
        throw std::exception("wrong quote");
      } else {
        const std::size_t endStringPosition = quotePosition + 1;
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

        // add character
        result.push_back(line.substr(beginPosition + length, 1));

        beginPosition += length + 1;
        length = 0;
      } else {
        // add character
        result.push_back(line.substr(beginPosition, 1));
        beginPosition += 1;
      }
    } else {
      length += 1;
    }
  }
  return result;
}

std::vector<std::string> splitLines(std::istream& input) {
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(input, line)) {
    lines.push_back(line);
  }
  return lines;
}

std::vector<std::vector<std::string> > split(std::istream& input) {
  std::vector<std::vector<std::string> > tokens;

  const std::vector<std::string>& lines = splitLines(input);

  for (unsigned int i = 0; i < lines.size(); ++i) {
    tokens.push_back(splitLine(lines[i]));
  }
  return tokens;
}

// TODO find count of vector
SToken getToken(const std::string& stringToken) {
  assert(!stringToken.empty());

  // scan for standard tokens
  for (int i = 0; i < sCategories.size(); ++i) {
    if (!_strcmpi(stringToken.c_str(), sCategories[i])) {
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
      throw std::exception(("wrong number : " + stringToken).c_str());
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

std::vector<std::vector<SToken> > parse(std::istream& input) {
  const std::vector<std::vector<std::string> >& tokens = split(input);

  std::vector<std::vector<SToken> > result;

  for (unsigned int i = 0; i < tokens.size(); ++i) {
    std::vector<SToken> lineTokens;

    for (unsigned int j = 0; j < tokens[i].size(); ++j) {
      if (!tokens[i][j].empty()) {
        lineTokens.push_back(getToken(tokens[i][j]));
      }
    }
    result.push_back(lineTokens);
  }
  return result;
}
}  // namespace NPeProtector