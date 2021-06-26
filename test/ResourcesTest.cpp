#define BOOST_TEST_MODULE resources test
#include <boost/test/included/unit_test.hpp>

#include "pe-protector/ClientFile.h"
#include "pe-protector/Resources.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(ResourcesTest);
BOOST_AUTO_TEST_CASE(testGetResourcesSizeEmpty) {
  SClientFile clientFile;
  const int size = getResourcesSize(clientFile);
  BOOST_TEST(size == 16);
}

BOOST_AUTO_TEST_CASE(testGetResourcesSizeManifest) {
  SClientFile clientFile;
  clientFile.mManifest = {1, 2, 3};

  const int size = getResourcesSize(clientFile);
  BOOST_TEST(size == 104);
}

BOOST_AUTO_TEST_CASE(testGetResourcesSizeIconds) {
  SClientFile clientFile;
  clientFile.mIcons = {{1, 2, 3}, {4, 5, 6}};
  clientFile.mGroupIcons = {1, 2, 3};

  const int size = getResourcesSize(clientFile);
  BOOST_TEST(size == 256);
}

BOOST_AUTO_TEST_CASE(testGetResourcesSizeAll) {
  SClientFile clientFile;
  clientFile.mIcons = {{1, 2, 3}, {4, 5, 6}};
  clientFile.mGroupIcons = {1, 2, 3};
  clientFile.mManifest = {1, 2, 3};

  const int size = getResourcesSize(clientFile);
  BOOST_TEST(size == 344);
}
BOOST_AUTO_TEST_SUITE_END();