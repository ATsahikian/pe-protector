#define BOOST_TEST_MODULE import test
#include <boost/test/included/unit_test.hpp>

#include "pe-protector/Import.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(ImportTest);

BOOST_AUTO_TEST_CASE(testResolveImport1) {
  SCommand command1;
  command1.mType = NCommand::IMPORT;
  command1.mImport.mDllName = "dll1";
  command1.mImport.mFunctionName = "func1";

  std::vector<SCommand> commands = {
      command1,
  };

  resolveImport(commands, 0);

  BOOST_TEST(commands[0].mRVA == 48);
}

BOOST_AUTO_TEST_CASE(testResolveImport2) {
  SCommand command1;
  command1.mType = NCommand::IMPORT;
  command1.mImport.mDllName = "dll1";
  command1.mImport.mFunctionName = "func1";

  SCommand command2;
  command2.mType = NCommand::IMPORT;
  command2.mImport.mDllName = "dll2";
  command2.mImport.mFunctionName = "func2";

  std::vector<SCommand> commands = {command1, command2};

  resolveImport(commands, 0);

  BOOST_TEST(commands[0].mRVA == 76);
  BOOST_TEST(commands[1].mRVA == 84);
}

BOOST_AUTO_TEST_CASE(testGetImportSize1) {
  SCommand command1;
  command1.mType = NCommand::IMPORT;
  command1.mImport.mDllName = "dll1";
  command1.mImport.mFunctionName = "func1";

  std::vector<SCommand> commands = {command1};

  const int size = getImportSize(commands);

  BOOST_TEST(size == 69);
}

BOOST_AUTO_TEST_CASE(testGetImportSize2) {
  SCommand command1;
  command1.mType = NCommand::IMPORT;
  command1.mImport.mDllName = "dll1";
  command1.mImport.mFunctionName = "func1";

  SCommand command2;
  command2.mType = NCommand::IMPORT;
  command2.mImport.mDllName = "dll2";
  command2.mImport.mFunctionName = "func2";

  std::vector<SCommand> commands = {command1, command2};

  const int size = getImportSize(commands);

  BOOST_TEST(size == 118);
}
BOOST_AUTO_TEST_SUITE_END();