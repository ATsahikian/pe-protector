#define BOOST_TEST_MODULE pe header test
#include <boost/test/included/unit_test.hpp>

#include "pe-protector/PeHeader.h"

using namespace NPeProtector;

BOOST_AUTO_TEST_SUITE(PeHeaderTest);
BOOST_AUTO_TEST_CASE(testGetHeaderSize) {
  BOOST_TEST(0x400 == getHeaderSize());
}
BOOST_AUTO_TEST_SUITE_END();
