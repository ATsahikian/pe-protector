#define BOOST_TEST_MODULE data test
#include <boost/test/included/unit_test.hpp>

#include "iostream"
#include "pe-protector/Data.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(DataTest);

BOOST_AUTO_TEST_CASE(testGetDataSize1) {
  std::vector<SLabel> labels;
  std::vector<SConstant> constants = {SConstant{labels, 10}};

  SData data{"", 1, constants, 2};

  const int size = getDataSize(data);

  BOOST_TEST(size == 2);
}  // namespace Test

BOOST_AUTO_TEST_CASE(testGetDataSize2) {
  std::vector<SLabel> labels;
  std::vector<SConstant> constants = {SConstant{labels, 10}};

  SData data{"", 4, constants, 2};

  const int size = getDataSize(data);

  BOOST_TEST(size == 8);
}

BOOST_AUTO_TEST_CASE(testPutData1) {
  std::stringstream expectedResult;
  char characters[] = {33};
  expectedResult.write(characters, sizeof(characters));

  std::vector<SLabel> labels;
  std::vector<SConstant> constants = {SConstant{labels, 33}};
  SData data{"", 1, constants, 1};

  std::stringstream result;

  putData(result, data, std::vector<SCommand>{});

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutData2) {
  std::stringstream expectedResult;
  char characters[] = {33, 33, 33};
  expectedResult.write(characters, sizeof(characters));

  std::vector<SLabel> labels;
  std::vector<SConstant> constants = {SConstant{labels, 33}};
  SData data{"", 1, constants, 3};

  std::stringstream result;

  putData(result, data, std::vector<SCommand>{});

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutData3) {
  std::stringstream expectedResult;
  char characters[] = {33, 34, 35};
  expectedResult.write(characters, sizeof(characters));

  std::vector<SLabel> labels;
  std::vector<SConstant> constants = {
      SConstant{labels, 33}, SConstant{labels, 34}, SConstant{labels, 35}};
  SData data{"", 1, constants, 1};

  std::stringstream result;

  putData(result, data, std::vector<SCommand>{});

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}

BOOST_AUTO_TEST_CASE(testPutData4) {
  std::stringstream expectedResult;
  DWORD characters[] = {33, 34, 35};
  expectedResult.write((char*)characters, sizeof(characters));

  std::vector<SLabel> labels;
  std::vector<SConstant> constants = {
      SConstant{labels, 33}, SConstant{labels, 34}, SConstant{labels, 35}};
  SData data{"", 4, constants, 1};

  std::stringstream result;

  putData(result, data, std::vector<SCommand>{});

  BOOST_TEST(result.rdbuf()->str() == expectedResult.rdbuf()->str());
}
BOOST_AUTO_TEST_SUITE_END();