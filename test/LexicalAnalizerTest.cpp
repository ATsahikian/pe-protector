#define BOOST_TEST_MODULE lexical analizer test
#include <boost/test/included/unit_test.hpp>

#include "compiler/CLexicalAnalizer.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(LexicalAnalizerTest);
BOOST_AUTO_TEST_CASE(testParseComma1) {
  std::stringstream input;
  input << ",";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::COMMA);
}

BOOST_AUTO_TEST_CASE(testParseComma2) {
  std::stringstream input;
  input << ",123";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::COMMA);
  BOOST_TEST(tokens[0][1].mType == NCategory::CONSTANT);
}

BOOST_AUTO_TEST_CASE(testParseComma3) {
  std::stringstream input;
  input << "123,";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::CONSTANT);
  BOOST_TEST(tokens[0][1].mType == NCategory::COMMA);
}

BOOST_AUTO_TEST_CASE(testParseKeywords) {
  std::stringstream input;
  input << "IMPORT EXTERN DUP PTR SECTION DIRECTIVE";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::IMPORT);
  BOOST_TEST(tokens[0][1].mType == NCategory::EXTERN);
  BOOST_TEST(tokens[0][2].mType == NCategory::DUP);
  BOOST_TEST(tokens[0][3].mType == NCategory::PTR);
  BOOST_TEST(tokens[0][4].mType == NCategory::SECTION);
  BOOST_TEST(tokens[0][5].mType == NCategory::DIRECTIVE);
}

BOOST_AUTO_TEST_CASE(testParseTrancate) {
  std::stringstream input;
  input << "  1 \n  2 \n  3";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::CONSTANT);
  BOOST_TEST(tokens[1][0].mType == NCategory::CONSTANT);
  BOOST_TEST(tokens[2][0].mType == NCategory::CONSTANT);
}

BOOST_AUTO_TEST_CASE(testParseName) {
  std::stringstream input;
  input << "_name";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::NAME);
  BOOST_TEST(tokens[0][0].mData == "_name");
}

BOOST_AUTO_TEST_CASE(testParseString) {
  std::stringstream input;
  input << "\"0-0\"";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::STRING);
  BOOST_TEST(tokens[0][0].mData == "0-0");
}

BOOST_AUTO_TEST_CASE(testParseConstant) {
  std::stringstream input;
  input << "555";

  const std::vector<std::vector<SToken> > tokens = parse(input);
  BOOST_TEST(tokens[0][0].mType == NCategory::CONSTANT);
  BOOST_TEST(tokens[0][0].mConstant == 555);
}
BOOST_AUTO_TEST_SUITE_END();